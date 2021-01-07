#ifndef ACCDNN_H
#define ACCDNN_H

struct accdnn_dev {
    dev_t dev_type;
//    struct mutex mutex;
    struct cdev cdev;
    struct class *accdnn_class;
    struct platform_device *pdev;
    void __iomem *virtaddr;
    phys_addr_t phy_addr_start;
    phys_addr_t phy_addr_end;
    int irq_num;
};

/* Register offset */
#define ACCDNN_CTRL_OFST   0x0000
#define ACCDNN_STAT_OFST   0x0004
#define ACCDNN_SRC_OFST    0x0008
#define ACCDNN_DST_OFST    0x000C

/* Register bits masks */
#define ACCDNN_CTRL_START_MASK  0x00000001  /* Start inferrence */
#define ACCDNN_CTRL_EINT_MASK   0x00000002  /* Enable interrupt */
#define ACCDNN_CTRL_RST_MASK    0x00000004  /* Reset module */

#define ACCDNN_STAT_READY_MASK  0x00000001  /* Ready to start */
#define ACCDNN_STAT_DONE_MASK   0x00000002  /* Inference finished */

/* IOCTL defines */
#define ACCDNN_IOCTL_BASE   'A'
#define ACCDNN_START_MODULE     _IOW(ACCDNN_IOCTL_BASE, 0, unsigned int)
#define ACCDNN_RESET_MODULE     _IOW(ACCDNN_IOCTL_BASE, 1, unsigned int)
#define ACCDNN_IS_READY         _IOR(ACCDNN_IOCTL_BASE, 2, unsigned int*)
#define ACCDNN_IS_DONE          _IOR(ACCDNN_IOCTL_BASE, 3, unsigned int*)

#endif