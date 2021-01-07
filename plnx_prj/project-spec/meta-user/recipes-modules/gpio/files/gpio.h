#ifndef GPIO_H
#define GPIO_H

/* Register offset */
#define DATA1_OFST  0x0000
#define DIR1_OFST   0x0004
#define DATA2_OFST  0x0008
#define DIR2_OFST   0x000C
#define GIER_OFST   0x011C
#define ISR_OFST    0x0120
#define IER_OFST    0x0128

/* IOCTL defines */
#define GPIO_IOCTL_BASE 'G'
#define GPIO_GET_PB     _IOR(GPIO_IOCTL_BASE, 0, unsigned int*)
#define GPIO_GET_LED    _IOR(GPIO_IOCTL_BASE, 1, unsigned int*)
#define GPIO_SET_LED    _IOW(GPIO_IOCTL_BASE, 2, unsigned int)

#endif