/*  accdnn.c - The simplest kernel module.

* Copyright (C) 2013 - 2016 Xilinx, Inc
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.

*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program. If not, see <http://www.gnu.org/licenses/>.

*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/dma-mapping.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include "accdnn.h"

/* Standard module information, edit as appropriate */
MODULE_LICENSE("GPL");
MODULE_AUTHOR
    ("Fan Cen <cenfan@outlook.com>");
MODULE_DESCRIPTION
    ("accdnn - FPGA inference accelerator driver module");

#define DRIVER_NAME "accdnn"

/* Simple example of how to receive command line parameters to your module.
   Delete if you don't need them */
unsigned myint = 0xdeadbeef;
char *mystr = "default";

module_param(myint, int, S_IRUGO);
module_param(mystr, charp, S_IRUGO);

struct accdnn_dev *accdnn_lp = NULL;

static irqreturn_t accdnn_irq(int irq, void *lp)
{
	printk("accdnn interrupt\n");
	return IRQ_HANDLED;
}

/* file operation */
int accdnn_open(struct inode *inode, struct file *filep)
{
	printk("accdnn open\n");
	return 0;
}

int accdnn_release(struct inode *inode, struct file *filep)
{
	printk("accdnn release\n");
	return 0;
}

ssize_t accdnn_read(struct file *filep, char __user *buf,
					size_t count, loff_t *offset)
{
	printk("accdnn read\n");
	return count;
}

ssize_t accdnn_write(struct file *filep, const char __user *buf,
					 size_t count, loff_t *offset)
{
	printk("accdnn write\n");
	return 0;
}

static long accdnn_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	switch (cmd)
	{
	case ACCDNN_START_MODULE:
		break;
	case ACCDNN_RESET_MODULE:
		break;
	case ACCDNN_IS_READY:
		break;
	case ACCDNN_IS_DONE:
		break;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

struct file_operations accdnn_fops = {
	.owner = THIS_MODULE,
	.open = accdnn_open,
	.release = accdnn_release,
	.read = accdnn_read,
	.write = accdnn_write,
	.unlocked_ioctl = accdnn_ioctl,
};

static int accdnn_probe(struct platform_device *pdev)
{
	int status = 0;
	struct resource *r_irq; /* Interrupt resources */
	struct resource *r_mem; /* IO mem resources */
	struct device *dev = &pdev->dev;

	accdnn_lp = (struct accdnn_dev *)kmalloc(sizeof(struct accdnn_dev), GFP_KERNEL);
	if (accdnn_lp == NULL) {
		dev_err(dev, "Cound not allocate accdnn device\n");
		return -ENOMEM;
	}
	memset(accdnn_lp, 0, sizeof(struct accdnn_dev));
    accdnn_lp->pdev = pdev;

	/* Get iospace for the device */
	r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r_mem) {
		dev_err(dev, "invalid address\n");
		return -ENODEV;
	}

	accdnn_lp->phy_addr_start = r_mem->start;
	accdnn_lp->phy_addr_end = r_mem->end;

	if (!request_mem_region(accdnn_lp->phy_addr_start,
				accdnn_lp->phy_addr_end - accdnn_lp->phy_addr_start + 1,
				DRIVER_NAME)) {
		dev_err(dev, "Couldn't lock memory region at %p\n",
			(void *)accdnn_lp->phy_addr_start);
		status = -EBUSY;
		goto error1;
	}

	accdnn_lp->virtaddr = ioremap(accdnn_lp->phy_addr_start, 
		accdnn_lp->phy_addr_end - accdnn_lp->phy_addr_start + 1);
	if (!accdnn_lp->virtaddr) {
		dev_err(dev, "accdnn: Could not allocate iomem\n");
		status = -EIO;
		goto error2;
	}

    /* Get a range of major numbers */
    status = alloc_chrdev_region(&accdnn_lp->dev_type, 0, 1, DRIVER_NAME);
    if (status < 0) {
        dev_err(dev, "unable to alloc chrdev\n");
        goto error3;
    }
	
    /* Register with the kernel as a character device */
    cdev_init(&accdnn_lp->cdev, &accdnn_fops);
    accdnn_lp->cdev.owner = THIS_MODULE;
    accdnn_lp->cdev.ops = &accdnn_fops;

//    mutex_init(&accdnn_lp->mutex);
    status = cdev_add(&accdnn_lp->cdev, accdnn_lp->dev_type, 1);
    accdnn_lp->accdnn_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(accdnn_lp->accdnn_class,
				  NULL,
				  accdnn_lp->dev_type,
				  NULL,
				  DRIVER_NAME);

	/* Get IRQ for the device */
	r_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!r_irq) {
		dev_info(dev, "no IRQ found\n");
		dev_info(dev, "accdnn at 0x%08x mapped to 0x%08x\n",
			(unsigned int __force)accdnn_lp->phy_addr_start,
			(unsigned int __force)accdnn_lp->virtaddr);
		return 0;
	}
	accdnn_lp->irq_num = r_irq->start;
	status = request_irq(accdnn_lp->irq_num, &accdnn_irq, 0, DRIVER_NAME, accdnn_lp);
	if (status) {
		dev_err(dev, "Could not allocate interrupt %d.\n", accdnn_lp->irq_num);
		goto error4;
	}

	dev_info(dev, "accdnn at 0x%08x mapped to 0x%08x, irq=%d\n",
		(unsigned int __force)accdnn_lp->phy_addr_start,
		(unsigned int __force)accdnn_lp->virtaddr,
		accdnn_lp->irq_num);

	dev_info(dev, "AccDNN added\n");
	return 0;

	/* Clean up */
error4:
	cdev_del(&accdnn_lp->cdev);
	device_destroy(accdnn_lp->accdnn_class, accdnn_lp->dev_type);
	class_destroy(accdnn_lp->accdnn_class);
error3:
	iounmap(accdnn_lp->virtaddr);
error2:
	release_mem_region(accdnn_lp->phy_addr_start, 
		accdnn_lp->phy_addr_end - accdnn_lp->phy_addr_start + 1);
error1:
	kfree(accdnn_lp);
	return status;
}

static int accdnn_remove(struct platform_device *pdev)
{
	free_irq(accdnn_lp->irq_num, accdnn_lp);
	cdev_del(&accdnn_lp->cdev);
	device_destroy(accdnn_lp->accdnn_class, accdnn_lp->dev_type);
	class_destroy(accdnn_lp->accdnn_class);
    unregister_chrdev_region(accdnn_lp->dev_type, 1);
	iounmap(accdnn_lp->virtaddr);
	release_mem_region(accdnn_lp->phy_addr_start, 
		accdnn_lp->phy_addr_end - accdnn_lp->phy_addr_start + 1);
	kfree(accdnn_lp);
	return 0;
}

static struct of_device_id accdnn_of_match[] = {
	{ .compatible = "xlnx,model-dma-1.0", },
	{ /* end of list */ },
};
MODULE_DEVICE_TABLE(of, accdnn_of_match);

static struct platform_driver accdnn_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table	= accdnn_of_match,
	},
	.probe		= accdnn_probe,
	.remove		= accdnn_remove,
};

static int __init accdnn_init(void)
{
	printk("<1>Hello module world.\n");
	printk("<1>Module parameters were (0x%08x) and \"%s\"\n", myint,
	       mystr);

	return platform_driver_register(&accdnn_driver);
}


static void __exit accdnn_exit(void)
{
	platform_driver_unregister(&accdnn_driver);
	printk(KERN_ALERT "Goodbye module world.\n");
}

module_init(accdnn_init);
module_exit(accdnn_exit);
