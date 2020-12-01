
// SPDX-License-Identifer: GPL-2.0
/*
 * Copyright (C) 2020 Satori Takasawa. All rights reserved.
 */


#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>
MODULE_AUTHOR("Satori Takasawa and Ryuichi Ueda");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
	int i,n;
	char c;
	if(copy_from_user(&c, buf, sizeof(char)))
		return -EFAULT;

	//printk(KERN_INFO "receive %c\n", c);
	
	if(c == '0'){
		gpio_base[10] = 1 << 25;
		gpio_base[10] = 1 << 24;
		gpio_base[10] = 1 << 23;
		gpio_base[10] = 1 << 18;

	}else if (c == '1'){
		for(i = 0; i < 5; i++){
		n = 500 - 100*i;
		gpio_base[7] = 1 << 25;
		gpio_base[7] = 1 << 24;
		gpio_base[7] = 1 << 23;
		gpio_base[7] = 1 << 18;
		msleep(300);
		gpio_base[10] = 1 << 25;
		gpio_base[10] = 1 << 24;
		gpio_base[10] = 1 << 23;
		gpio_base[10] = 1 << 18;
		msleep(300);
	
        	}	
	}else if (c == '2'){
		for(i = 0; i < 3; i++){
		n = 300 - 100*i;
		gpio_base[7] = 1 << 25;
		msleep(n);
		gpio_base[10] = 1 << 25;
		gpio_base[7] = 1 << 24;
		msleep(n);
		gpio_base[10] = 1 << 24;
		gpio_base[7] = 1 << 23;
		msleep(n);
		gpio_base[10] = 1 << 23;
		gpio_base[7] = 1 << 18;
		msleep(n);
		gpio_base[10] = 1 << 18;
		}

	}else if (c == '3'){
		for(i = 0; i < 5; i++){
		n = 500 - 100*i;
		gpio_base[7] = 1 << 18;
		msleep(n);
		gpio_base[10] = 1 << 18;
		gpio_base[7] = 1 << 25;
		gpio_base[7] = 1 << 23;
		msleep(n);
		gpio_base[10] = 1 << 25;
		gpio_base[10] = 1 << 23;
		gpio_base[7] = 1 << 24;
		msleep(n);
		gpio_base[10] = 1 << 24;
		gpio_base[7] = 1 << 25;
		gpio_base[7] = 1 << 23;
		msleep(n);
		gpio_base[10] = 1 << 25;
		gpio_base[10] = 1 << 23;
		gpio_base[7] = 1 << 18;

		}
	}
	return 1;
}

static ssize_t sushi_read(struct file* filp, char* buf, size_t count, loff_t* pos)
{
	int size = 0;
	char sushi[] = {0xF0, 0x9F, 0x8D, 0xA3, 0x0A};
	if(copy_to_user(buf+size, (const char *)sushi, sizeof(sushi))){
		printk(KERN_ERR "sushi: copy_to_user failed.\n");
		return -EFAULT;
	}

	size += sizeof(sushi);
	return size;
} 

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
	.read = sushi_read
};

static int __init init_mod(void) //カーネモジュールの初期化
{
	int retval;
	retval = alloc_chrdev_region(&dev, 0, 1, "myled");
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}

	printk(KERN_INFO "%s is loaded. major:%d\n", __FILE__, MAJOR(dev));

	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdv, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d", MAJOR(dev), MINOR(dev));
		return retval;
	}

	cls = class_create(THIS_MODULE, "myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR"clas_create failed.");
		return PTR_ERR(cls);
	}

	device_create(cls, NULL, dev, NULL, "myled%d", MINOR(dev));

	gpio_base = ioremap_nocache(0xfe200000, 0xA0);

	const u32 led = 25;
	const u32 index = led/10;
	const u32 shift = (led%10)*3;
	const u32 mask = ~(0x7 << shift);
	gpio_base[index] = (gpio_base[index] & mask) | (0x1 << shift);

	const u32 led2 = 24;
	const u32 index2 = led/10;
	const u32 shift2 = (led%10)*3;
	const u32 mask2 = ~(0x7 << shift);
	gpio_base[index] = (gpio_base[index] & mask2) | (0x1 << shift2);

	const u32 led3 = 23;
	const u32 index3 = led/10;
	const u32 shift3 = (led%10)*3;
	const u32 mask3 = ~(0x7 << shift);
	gpio_base[index] = (gpio_base[index] & mask3) | (0x1 << shift3);

	const u32 led4 = 18;
	const u32 index4 = led/10;
	const u32 shift4 = (led%10)*3;
	const u32 mask4 = ~(0x7 << shift);
	gpio_base[index] = (gpio_base[index] & mask4) | (0x1 << shift4);
	return 0;
}

static void __exit cleanup_mod(void) //後始末
{
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n", __FILE__, MAJOR(dev));	
}

module_init(init_mod); //マクロで関数を登録
module_exit(cleanup_mod); //同上
