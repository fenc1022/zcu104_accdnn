#ifndef ACCDNN_H
#define ACCDNN_H

#define WGT_SIZE 292352 // predefined model weight size in bytes
#define IMG_SIZE 8192   // predefined image size in bytes
#define OUT_SIZE 4096   // predefined output size in bytes

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

struct accdnn_wgt_mem {
    unsigned long host_virtaddr;
    phys_addr_t   dev_phyaddr;
};

struct accdnn_io_mem {
    unsigned long   host_input_virtaddr;
    unsigned long   host_output_virtaddr;
};

/* Register offset */
#define ACCDNN_CTRL_OFST   0x0000   /* Device control register */
#define ACCDNN_STAT_OFST   0x0004   /* Device status register */
#define ACCDNN_DWGT_OFST   0x0008   /* Physical address of weights in device memory */
#define ACCDNN_HWGT_OFST   0x000C   /* Physical address of weights in host memory */
#define ACCDNN_SRC_OFST    0x0010   /* Physical address of input data in host memory */
#define ACCDNN_DST_OFST    0x0014   /* Physical address of output data in host memory */
#define ACCDNN_SIZE_OFST   0x0018   /* Number of images to be infered */

/* Register bits masks */
#define ACCDNN_CTRL_START_MASK  0x00000001  /* Start inferrence */
#define ACCDNN_CTRL_LOAD_MASK   0x00000002  /* Start load weight */
#define ACCDNN_CTRL_EINT_MASK   0x00000004  /* Enable interrupt */
#define ACCDNN_CTRL_RST_MASK    0x00000008  /* Reset device */

#define ACCDNN_STAT_READY_MASK  0x00000001  /* Ready to start */
#define ACCDNN_STAT_BUSY_MASK   0x00000002  /* Inference finished */
#define ACCDNN_STAT_DONE_MASK   0x00000004  /* Device running */

/* IOCTL defines */
#define ACCDNN_IOCTL_BASE   'A'
#define ACCDNN_RESET_MODULE     _IO(ACCDNN_IOCTL_BASE, 0)
#define ACCDNN_LOAD_WEIGHTS     _IOW(ACCDNN_IOCTL_BASE, 1, struct accdnn_wgt_mem*)
#define ACCDNN_CFG_IRQ          _IOW(ACCDNN_IOCTL_BASE, 2, unsigned int)
#define ACCDNN_START_INF        _IOW(ACCDNN_IOCTL_BASE, 3, struct accdnn_io_mem*)
#define ACCDNN_CHECK_STAT       _IOR(ACCDNN_IOCTL_BASE, 4, unsigned int*)

#endif
