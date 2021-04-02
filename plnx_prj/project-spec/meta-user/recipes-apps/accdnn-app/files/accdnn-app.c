/*
* Copyright (C) 2013 - 2016  Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in this
* Software without prior written authorization from Xilinx.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "accdnn-app.h"

int main(int argc, char **argv)
{
    int fd_dev, fd_wgt, fd_in, fd_out;
    unsigned int img_num;
    void *buf_wgt = NULL;
    void *buf_in = NULL;
    void *buf_out = NULL;
    struct accdnn_host_mem host_mem;
    struct timespec start, stop;
    double time_diff;

    if ((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
        printf("Usage: %s input_bin_file_name [image_number]\n", argv[0]);
        return 0;
    }

    img_num = (argc > 2) ? atoi(argv[2]) : 1;
    printf("AccDNN demo!\n");
    printf("%d images from %s\n", img_num, argv[1]);

    // Open FPGA accelerator device & files
    fd_dev = open("/dev/accdnn", O_RDWR);
    if (fd_dev == -1) {
        printf("Error while opening device\n");
        return -1;
    }

    fd_wgt = open("weights.bin", O_RDONLY);
    if (fd_wgt == -1) {
        printf("Error while opening weights file\n");
        return -1;
    }

    fd_in = open(argv[1], O_RDONLY);
    if (fd_in == -1) {
        printf("Error while opening input file\n");
        return -1;
    }

    fd_out = open("data_out.dat", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd_out == -1) {
        printf("Error while opening output file\n");
        return -1;
    }

    // Memory allocation
    buf_wgt =  malloc(WGT_SIZE);
    if (read(fd_wgt, buf_wgt, WGT_SIZE) == -1) {
        printf("Failed to read weights file\n");
        return -1;
    }

    buf_in = malloc(IMG_SIZE * img_num);
    if (read(fd_in, buf_in, IMG_SIZE * img_num) == -1) {
        printf("Failed to read input file\n");
        return -1;
    }

    buf_out = malloc(OUT_SIZE * img_num);

    host_mem.weight_virtaddr = (unsigned long long)buf_wgt;
    host_mem.input_virtaddr = (unsigned long long)buf_in;
    host_mem.output_virtaddr = (unsigned long long)buf_out;
    host_mem.img_num = img_num;

    // Load weights to device memory
    if (ioctl(fd_dev, ACCDNN_LOAD_WEIGHTS, &host_mem) != 0) {
        printf( "Failed to load weights file\n" );
        return -1;
    }

    // Start inference
    if ( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
      printf( "error in clock gettime start\n" );
    }

    if (ioctl(fd_dev, ACCDNN_START_INF, &host_mem) != 0) {
        printf("Failed to launch inference\n");
        return -1;
    };

    if ( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      printf( "error in clock gettime stop\n" );
    }

    time_diff = (stop.tv_sec - start.tv_sec) +
                (stop.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    // Store result
    write(fd_out, buf_out, OUT_SIZE * img_num);

    // Clean up
    free(buf_out);
    free(buf_in);
    free(buf_wgt);

    if (close(fd_out) == -1) {
        return -1;
    }

    if (close(fd_in) == -1) {
        return -1;
    }

    if (close(fd_wgt) == -1) {
        return -1;
    }

    if (close(fd_dev) == -1) {
        return -1;
    }
    
    printf("In user space: \n");
    printf("%d images are inferenced in %.3f secs\n", img_num, time_diff);
    printf("Throughput: %.1f fps\n", img_num / time_diff);

    return 0;
}
