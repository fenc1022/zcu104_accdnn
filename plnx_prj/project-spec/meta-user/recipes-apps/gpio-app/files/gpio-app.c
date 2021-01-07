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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "gpio.h"

int main(int argc, char **argv)
{
    int fd, size;
    unsigned int led = 0x5555AAAA;
    unsigned int pb = 0x5555AAAA;
    printf("GPIO driver test\n");
    fd = open("/dev/axi_gpio", O_RDWR);
    if (fd == -1) {
        printf("Error while opening gpio\n");
        return -1;
    }

    size = read(fd, &led, 4);
    size = write(fd, "Hello\n", strlen("Hello\n"));

    ioctl(fd, GPIO_GET_LED, &led);
    printf("LED is %x\n", led);
    ioctl(fd, GPIO_GET_PB, &pb);
    printf("Push-button is %x\n", pb);

    for (int i=0; i<4; i++) {
        sleep(1);
        led = ~led;
        ioctl(fd, GPIO_SET_LED, led);
    }

    ioctl(fd, GPIO_GET_PB, &pb);
    printf("Push-button is %x\n", pb);

    if (close(fd) < 0) {
        printf("Error while closing gpio\n");
    }

    return 0;
}
