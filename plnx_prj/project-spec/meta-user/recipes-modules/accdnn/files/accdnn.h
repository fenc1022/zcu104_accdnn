#ifndef ACCDNN_H
#define ACCDNN_H

#define WGT_SIZE 58291648 // predefined model weight size in bytes
#define IMG_SIZE 824512   // predefined image size in bytes
#define OUT_SIZE 64   // predefined output size in bytes
#define DEV_WGT_PHYADDR 0  // physical address of weights in device memory

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

struct accdnn_host_mem {
    unsigned long long  weight_virtaddr;
    unsigned long long  input_virtaddr;
    unsigned long long  output_virtaddr;
    unsigned int        img_num;
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

#define ACCDNN_STAT_HASWGT_MASK 0x00000001  /* Weights has been loaded */
#define ACCDNN_STAT_HASNUM_MASK 0x00000002  /* Image number has been set */
#define ACCDNN_STAT_BUSY_MASK   0x00000004  /* Processing */
#define ACCDNN_STAT_RDY_MASK    0x00000008  /* HASWGT & HASNUM & !BUSY */
#define ACCDNN_STAT_DONE_MASK   0x00000010  /* Inference finish */

/* IOCTL defines */
#define ACCDNN_IOCTL_BASE   'A'
#define ACCDNN_RESET_MODULE     _IO(ACCDNN_IOCTL_BASE, 0)
#define ACCDNN_LOAD_WEIGHTS     _IOW(ACCDNN_IOCTL_BASE, 1, struct accdnn_host_mem*)
#define ACCDNN_CFG_IRQ          _IOW(ACCDNN_IOCTL_BASE, 2, unsigned int)
#define ACCDNN_START_INF        _IOW(ACCDNN_IOCTL_BASE, 3, struct accdnn_host_mem*)
#define ACCDNN_CHECK_STAT       _IOR(ACCDNN_IOCTL_BASE, 4, unsigned int*)

#endif
