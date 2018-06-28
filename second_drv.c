#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mm.h>

#define MAJOR_NUM 112

static struct class		*seconddrv_class;
static struct class_device	*seconddrv_class_dev;
static struct cdev		*second_drv; 

static int second_drv_open(struct inode *my_inode, struct file *my_file)
{
	/* default to be level 4*/
	printk("second_drv_open\n");
	return 0;
}


static int second_drv_release(struct inode *my_inode, struct file *my_file)
{
	printk("second_drv_release\n");
	return 0;
}

static int second_drv_mmap(struct file *my_file, struct vm_area_struct * my_va)
{
	int ret;
	/*VM_IO会阻止操作系统将该内存区域包含在进程的存放转存(core dump)中,VM_RESERVED表示该内存区域不能被换出*/
	my_va->vm_flags |= VM_RESERVED | VM_IO;
	/*pgprot_noncached()是宏，它禁止了相关页的cache和写缓冲(write buffer)*/
	my_va->vm_page_prot = pgprot_noncached(my_va->vm_page_prot);
	ret = remap_pfn_range(my_va,my_va->vm_start,my_va->vm_pgoff,my_va->vm_end-my_va->vm_start,my_va->vm_page_prot);
	if (ret < 0)
		printk("remap_pfn_range err!\n");
	else
		printk("second_drv_mmap success!\n");

	return 0;
}


 static struct file_operations second_drv_fops=
 {
         .owner  =	THIS_MODULE,
         .open   =	second_drv_open,
         .release=	second_drv_release,
         .mmap 	 =	second_drv_mmap,
 };

static int __init second_drv_init(void)
{
    int err;
	dev_t devid;
	devid = MKDEV(MAJOR_NUM, 0);
	second_drv = cdev_alloc();
	cdev_init(second_drv, &second_drv_fops);
	err = cdev_add(second_drv, devid, 1);
 	if(err)
		return -1;

	/*old way
	int major;
	major = register_chrdev(0, "second_drv", &second_drv_fops); 
       */

	seconddrv_class = (struct class *)class_create(THIS_MODULE, "seconddrv");
	/* /dev/seconddrv */
	seconddrv_class_dev = (struct class_device *)class_device_create(seconddrv_class, NULL, devid, NULL, "seconddrv");

	return 0;
}


static void __exit second_drv_exit(void)
{
	cdev_del(second_drv);

	/*old way
	unregister_chrdev(major, "second_drv");
	*/ 
	
	class_device_unregister(seconddrv_class_dev);
	class_destroy(seconddrv_class);
}

module_init(second_drv_init);
module_exit(second_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John");
MODULE_DESCRIPTION("LED Ctrl 2");
MODULE_VERSION("1.00");
