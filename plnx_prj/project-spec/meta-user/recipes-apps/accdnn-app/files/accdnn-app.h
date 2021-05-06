#ifndef ACCDNN_APP_H
#define ACCDNN_APP_H

#include <asm-generic/ioctl.h>

#define WGT_SIZE 58291648 // predefined model weight size in bytes
#define IMG_SIZE 824512   // predefined image size in bytes
#define OUT_SIZE 64   // predefined output size in bytes

struct accdnn_host_mem {
    unsigned long long  weight_virtaddr;
    unsigned long long  input_virtaddr;
    unsigned long long  output_virtaddr;
    unsigned int        img_num;
};

/* IOCTL defines */
#define ACCDNN_IOCTL_BASE   'A'
#define ACCDNN_RESET_MODULE     _IO(ACCDNN_IOCTL_BASE, 0)
#define ACCDNN_LOAD_WEIGHTS     _IOW(ACCDNN_IOCTL_BASE, 1, struct accdnn_host_mem*)
#define ACCDNN_CFG_IRQ          _IOW(ACCDNN_IOCTL_BASE, 2, unsigned int)
#define ACCDNN_START_INF        _IOW(ACCDNN_IOCTL_BASE, 3, struct accdnn_host_mem*)
#define ACCDNN_CHECK_STAT       _IOR(ACCDNN_IOCTL_BASE, 4, unsigned int*)

#endif
