#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include "gpio.h"


/* Data structure to describe the device */
struct axi_gpio {
    dev_t devt;
    struct cdev cdev;
    struct class *axi_gpio_class;
    struct platform_device *pdev;
    struct resource *res;
    void __iomem *__vaddr;
//    struct mutex dev_mutex;
};

struct axi_gpio *axi_gpio_dev;
int irq_num;

static irqreturn_t gpio_isr(int irq, void* dev_id)
{
    int data;
    
    printk("Enter axi gpio interrupt service...\n");
    /* Light LED according to push-button   */
    data = ioread32(axi_gpio_dev->__vaddr + DATA2_OFST);
    iowrite32(data, axi_gpio_dev->__vaddr + DATA1_OFST);
    /* Clear interrupt status */
    iowrite32(0x00000002, axi_gpio_dev->__vaddr + ISR_OFST);

    return IRQ_HANDLED;
}

/* file operation functions */
int gpio_open(struct inode *inode, struct file *filp)
{
    /* Set GPIO dirextion */
    iowrite32(0x0, axi_gpio_dev->__vaddr + DIR1_OFST);
    iowrite32(0xFFFFFFFF, axi_gpio_dev->__vaddr + DIR2_OFST);
    dev_info(&axi_gpio_dev->pdev->dev, "GPIO opened\n");
    return 0;
}

int gpio_release(struct inode *inode, struct file *filp)
{
    dev_info(&axi_gpio_dev->pdev->dev, "GPIO released\n");
    return 0;
}

ssize_t gpio_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *offset)
{
    dev_info(&axi_gpio_dev->pdev->dev, "Nothing to read\n");
    return 0;
}

ssize_t gpio_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *offset)
{
    dev_info(&axi_gpio_dev->pdev->dev, "Cannot write here\n");
    return count;
}

static long gpio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    unsigned int push_button = 0;
    unsigned int led = 0;
    unsigned long long arg64 = 0;

    arg64 = (unsigned long long)arg;

    switch (cmd)
    {
    case GPIO_GET_PB:
        push_button = ioread32(axi_gpio_dev->__vaddr + DATA2_OFST);
        copy_to_user((int*)arg64, &push_button, sizeof(int));
        break;
    case GPIO_GET_LED:
        led = ioread32(axi_gpio_dev->__vaddr + DATA1_OFST);
        copy_to_user((int*)arg64, &led, sizeof(int));
        break;
    case GPIO_SET_LED:
        iowrite32(arg, axi_gpio_dev->__vaddr + DATA1_OFST);
        break;
    default:
        return -ENOTTY;
    }
    
    return 0;
}

struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
    .open = gpio_open,
    .release = gpio_release,
    .read = gpio_read,
    .write = gpio_write,
    .unlocked_ioctl = gpio_ioctl
};

static int gpio_pdrv_probe(struct platform_device *pdev)
{
    int status = 0;
    struct device *gpio_device;
    pr_info("Entering probe function\n");
    struct device_node *np;

    /*Allocate device node */
    np = pdev->dev.of_node;

    /* Allocate a private structure to manage this device */
    axi_gpio_dev = kmalloc(sizeof(struct axi_gpio), GFP_KERNEL);
    if (axi_gpio_dev == NULL) {
        dev_err(&pdev->dev, "unable to allocate device structure\n");
        return -ENOMEM;
    }
    memset(axi_gpio_dev, 0 , sizeof(struct axi_gpio));

    /* IOMAP device registers */
    axi_gpio_dev->res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    axi_gpio_dev->__vaddr = devm_ioremap_resource(&pdev->dev, axi_gpio_dev->res);
    if (!axi_gpio_dev->__vaddr) {
        dev_err(&pdev->dev, "unable to IOMAP gpio registers\n");
        status = -ENOMEM;
        goto fail1;
    }

    /* Get a range of major numbers */
    axi_gpio_dev->pdev = pdev;
    status = alloc_chrdev_region(&axi_gpio_dev->devt, 0, 1, "axi_gpio");
    if (status < 0) {
        dev_err(&pdev->dev, "unable to alloc chrdev\n");
        goto fail2;
    }

    /* Create device class */
    axi_gpio_dev->axi_gpio_class = class_create(THIS_MODULE, "axi_gpio_class");
    if (IS_ERR(axi_gpio_dev->axi_gpio_class)) {
        dev_err(&pdev->dev, "unable to create device class\n");
        status = -ENOMEM;
        goto fail3;
    }

    /* Initialize the char device and tie a file_operations to it */
    cdev_init(&axi_gpio_dev->cdev, &gpio_fops);
    axi_gpio_dev->cdev.owner = THIS_MODULE;

    // mutex_init(&axi_gpio_dev->mutex);
    cdev_add(&axi_gpio_dev->cdev, axi_gpio_dev->devt, 1);

    gpio_device = device_create(
                    axi_gpio_dev->axi_gpio_class,
                    NULL,   /* no parent device */
                    axi_gpio_dev->devt,    /* associated dev_t */
                    NULL,   /* no additional data */
                    "axi_gpio");  /* device name */

    if (IS_ERR(gpio_device)) {
        dev_err(&pdev->dev, "unable to create device\n");
        status = -ENOMEM;
        goto fail4;
    }

    /* Register ISR & enable interrupt */
    irq_num = platform_get_irq(pdev, 0);
//    irq_num = irq_of_parse_and_map(np, 0);
    if (request_irq(irq_num, gpio_isr, 0, dev_name(&pdev->dev), NULL)) {
        dev_err(&pdev->dev, "unable to register irq\n");
        return -EIO;
    }
    iowrite32(0x80000000, axi_gpio_dev->__vaddr + GIER_OFST);
    iowrite32(0x00000002, axi_gpio_dev->__vaddr + IER_OFST);

    dev_info(&pdev->dev, "AXI GPIO added\n");
    return 0;

    /* Clean up */
    fail4:
        class_destroy(axi_gpio_dev->axi_gpio_class);
    fail3:
        unregister_chrdev_region(axi_gpio_dev->devt, 1);
    fail2:
        iounmap(axi_gpio_dev->__vaddr);
    fail1:
        kfree(axi_gpio_dev);
        return status;
}

static int gpio_pdrv_remove(struct platform_device *pdev)
{
    free_irq(irq_num, NULL);
    cdev_del(&axi_gpio_dev->cdev);
    device_destroy(axi_gpio_dev->axi_gpio_class, axi_gpio_dev->devt);
    class_destroy(axi_gpio_dev->axi_gpio_class);
    unregister_chrdev_region(axi_gpio_dev->devt, 1);

    if (axi_gpio_dev->__vaddr) {
        iounmap(axi_gpio_dev->__vaddr);
    }

    if (axi_gpio_dev) {
        kfree(axi_gpio_dev);
    }

    dev_info(&pdev->dev, "AXI GPIO unloaded\n");
    return 0;
}

static const struct of_device_id axi_gpio_dt_ids[] = {
    {.compatible = "xlnx,axi-gpio-2.0",},
    { /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, axi_gpio_dt_ids);

static struct platform_driver gpio_pdrv = {
    .probe  = gpio_pdrv_probe,
    .remove = gpio_pdrv_remove,
    .driver = {
        .name = "axi_gpio_0",
        .of_match_table = axi_gpio_dt_ids,
        .owner = THIS_MODULE,
    },
};  

module_platform_driver(gpio_pdrv);

MODULE_AUTHOR("Fan Cen <cenfan@outlook.com>");
MODULE_DESCRIPTION("My axi gpio platform driver");
MODULE_LICENSE("GPL");