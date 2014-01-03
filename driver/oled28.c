#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#define DRIVER_AUTHOR "Saak Dertadian <s.dertadian@gmail.com>"
#define DRIVER_DESC   "First Led Test Driver"
MODULE_LICENSE("GPL");

#define TIMER_PERIOD	250	// * 1 ms
unsigned int GPIO_DC=49;
unsigned int GPIO_RS=60;

static struct timer_list my_timer;
int tt;

void my_timer_callback( unsigned long data )
{
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_PERIOD));
	tt++;
//	printk(KERN_EMERG "Hello: tt=%d\n",tt);
// 	gpio_set_value(LED_GPIO_1,tt&0x01);
// 	if(flagBp)
// 	{
// 		flagBp=0;
// 		gpio_set_value(LED_GPIO_2,1);
// 	}
// 	else
// 		gpio_set_value(LED_GPIO_2,0);
}

int init_module(void)
{
	int ret;

	tt=0;

	printk(KERN_EMERG "Init Oled28.\n");

	ret=gpio_request(GPIO_DC,"Oled DC 1");
	if(ret)	printk(KERN_EMERG "Error: gpio_request: %d\n",GPIO_DC);
	ret=gpio_request(GPIO_RS,"Oled RS");
	if(ret)	printk(KERN_EMERG "Error: gpio_request: %d\n",GPIO_RS);

	gpio_direction_output(GPIO_DC,1);
	gpio_direction_output(GPIO_RS,1);
	gpio_set_value(GPIO_DC,0);
	gpio_set_value(GPIO_RS,0);

	/* setup your timer to call my_timer_callback */
	setup_timer(&my_timer, my_timer_callback, 0);
	/* setup timer interval to 1000 msecs */
	ret=mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_PERIOD));
	if(ret)	printk("Error in mod_timer\n");

	return 0;
}

void cleanup_module(void)
{
	/* remove kernel timer when unloading module */
	del_timer(&my_timer);

	gpio_free(GPIO_DC);
	gpio_free(GPIO_RS);

	printk(KERN_EMERG "Close Oled28.\n");
}
