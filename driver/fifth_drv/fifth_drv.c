#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/poll.h>

#define BTN_DELY	5		/* 5 seconds */ 

int major;
 
static struct class *fifthdrv_class;
static struct device *fifthdrv_dev;
 
//volatile unsigned long *gpfcon;
//volatile unsigned long *gpfdat;
 
static DECLARE_WAIT_QUEUE_HEAD(button_waitq);
 
/* 中断事件标志, 中断服务程序将它置1，fifth_drv_read将它清0 */
static volatile int ev_press = 0;
 
static struct fasync_struct *button_async; 
 
/* 键值: 按下时, 0x01*/
/* 键值: 松开时, 0x81*/
static unsigned char key_val;
  
struct timer_list mytimer; 
 
/*
  * 确定按键值
  */
static void button_fake_irq(struct timer_list *tmr)
{
	if (key_val & 0x80) {
		/* 按下 */
		key_val = 0x01;
	} else {
		/* 松开 */
		key_val = 0x80 | 0x01;
	}	
 
    ev_press = 1;                  /* 表示中断发生了 */
    wake_up_interruptible(&button_waitq);   /* 唤醒休眠的进程 */
	
    //发送信号SIGIO信号给fasync_struct 结构体所描述的PID，触发应用程序的SIGIO信号处理函数
	kill_fasync (&button_async, SIGIO, POLL_IN);
	
	mod_timer(tmr, jiffies + BTN_DELY*HZ);

	return ;
}
 
static int fifth_drv_open(struct inode *inode, struct file *file)
{ 
	printk("driver: %s\n", __FUNCTION__);
	timer_setup(&mytimer, button_fake_irq, 0);    
	mod_timer(&mytimer, jiffies + BTN_DELY*HZ);
  	
	return 0;
}
 
ssize_t fifth_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	if (size != 1)
		return -EINVAL;
 
	/* 如果没有按键动作, 休眠 */
	wait_event_interruptible(button_waitq, ev_press);
 
	/* 如果有按键动作, 返回键值 */
	copy_to_user(buf, &key_val, 1);
	ev_press = 0;
	
	return 1;
}
 
 
int fifth_drv_close(struct inode *inode, struct file *file)
{
	printk("driver: %s\n", __FUNCTION__);
	del_timer(&mytimer);
	
	return 0;
}
 
static unsigned fifth_drv_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	poll_wait(file, &button_waitq, wait); // 不会立即休眠
 
	if (ev_press)
		mask |= POLLIN | POLLRDNORM;
 
	return mask;
}
 
static int fifth_drv_fasync (int fd, struct file *filp, int on)
{
	printk("driver: %s\n", __FUNCTION__);
	//初始化/释放 fasync_struct 结构体 (fasync_struct->fa_file->f_owner->pid)
	return fasync_helper (fd, filp, on, &button_async);
}
 
 
static struct file_operations sencod_drv_fops = {
    .owner   =  THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open    =  fifth_drv_open,     
	.read	 =	fifth_drv_read,	   
	.release =  fifth_drv_close,
	.poll    =  fifth_drv_poll,
	.fasync	 =  fifth_drv_fasync,
};
  
static int fifth_drv_init(void)
{
	printk("driver: %s\n", __FUNCTION__);
	
	major = register_chrdev(0, "fifth_drv", &sencod_drv_fops);
 
	fifthdrv_class = class_create(THIS_MODULE, "fifth_drv");
 
	fifthdrv_dev = device_create(fifthdrv_class, NULL, MKDEV(major, 0), NULL, "buttons"); /* /dev/buttons */
 
	return 0;
}
 
static void fifth_drv_exit(void)
{
	printk("driver: %s\n", __FUNCTION__);
	
	unregister_chrdev(major, "fifth_drv");
	device_unregister(fifthdrv_dev);
	class_destroy(fifthdrv_class);

	return;
}
 
 
module_init(fifth_drv_init);
 
module_exit(fifth_drv_exit);
 
MODULE_LICENSE("GPL");

