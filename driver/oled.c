#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/unistd.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include "oled.h"

#define DRIVER_AUTHOR "Saak Dertadian <s.dertadian@gmail.com>"
#define DRIVER_DESC   "First Led Test Driver"
MODULE_LICENSE("GPL");

// unsigned int oled.gpio_dc=49;
// unsigned int GPIO_RS=60;

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

// 	u8 buf[5]={0x55, 1, 2, 3, 4 };
// 	spi_write(oled.spi,buf,1);
	
// 	if(tt&0x01)
// 		OledSetPixel(10,10,0xFFFF);
// 	else
// 		OledSetPixel(10,10,0x0000);
}

int OledInit()
{
	gpio_set_value(oled.gpio_rs,0);
	udelay(120);
	gpio_set_value(oled.gpio_rs,1);

	OledWriteCmd(CMD_SET_COMMAND_LOCK);
	OledWriteData(0x12);	// Unlock Basic Commands (0x12/0x16)
	
	OledWriteCmd(CMD_SET_DISPLAY_OFF);	// Display Off (0x00/0x01)
	
	OledWriteCmd(CMD_SET_COLUMN_ADDRESS);
	OledWriteData(0x1C);
	OledWriteData(0x5B);
	
	OledWriteCmd(CMD_SET_ROW_ADDRESS);
	OledWriteData(0x00);
	OledWriteData(0x3F);
	
	OledWriteCmd(CMD_SET_DISPLAY_CLOCK);
	OledWriteData(0x91);	// Set Clock as 80 Frames/Sec
	
	OledWriteCmd(CMD_SET_MULTIPLEX_RATIO);
	OledWriteData(0x3F);	// 1/64 Duty (0x0F~0x3F)
	
	OledWriteCmd(CMD_SET_DISPLAY_OFFSET);
	OledWriteData(0x00);	// Shift Mapping RAM Counter (0x00~0x3F)
	
	OledWriteCmd(CMD_SET_DISPLAY_START_LINE);
	OledWriteData(0x00);	// Set Mapping RAM Display Start Line (0x00~0x7F)
	
	OledWriteCmd(CMD_SET_REMAP);
	OledWriteData(0x14);	// 0x14
	OledWriteData(0x11);	// Enable dual COM mode
	
	OledWriteCmd(CMD_SET_GPIO);
	OledWriteData(0x00);	// Disable GPIO input pin
	
	OledWriteCmd(CMD_FUNCTION_SELECTION);
	OledWriteData(0x01);	// Enable internal VDD regulator

	OledWriteCmd(CMD_SET_DISPLAY_ENHANCEMENT_A);
	OledWriteData(0xA0);	// Set_Display_Enhancement_A(0xA0,0xFD);
	OledWriteData(0xFD);
	
	OledWriteCmd(CMD_SET_CONTRAST_CONTROL);
	OledWriteData(0x9F);	// Set Segment Output Current
	
	OledWriteCmd(CMD_MASTER_CONTRAST_CONTROL);
	OledWriteData(0x0F);	// Set Scale Factor of Segment Output Current Control 
	
	OledWriteCmd(CMD_SELECT_DEFAULT);	// set default linear gray scale table
	OledWriteCmd(CMD_ENABLE_GRAYSCALE_TABLE);
	
	OledWriteCmd(CMD_SET_PHASE_LENGTH);
	OledWriteData(0xE2);	// Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks

	OledWriteCmd(CMD_SET_DISPLAY_ENHANCEMENT_B);
	OledWriteData(0x20);	// Set_Display_Enhancement_B(0x20);
	
	OledWriteCmd(CMD_SET_PRECHARGE_VOLTAGE);
	OledWriteData(0x1F);	// Set Pre‐Charge Voltage Level as 0.60*VCC
	
	OledWriteCmd(CMD_SET_SECOND_PRECHARGE_PERIOD);
	OledWriteData(0x08);	// Set Second Pre‐Charge Period as 8 Clocks 
	
	OledWriteCmd(CMD_SET_VCOMH_VOLTAGE);
	OledWriteData(0x07);	// Set Common Pins Deselect Voltage Level as 0.86*VCC
	
	OledWriteCmd(CMD_DISPLAY_MODE_NORMAL);	// Normal Display Mode (0x00/0x01/0x02/0x03)
	
	OledWriteCmd(CMD_EXIT_PARTIAL_DISPLAY);
// 	OledWriteCmd(CMD_ENABLE_PARTIAL_DISPLAY);
// 	OledWriteData(0x00);	// Disable Partial Display
// 	OledWriteData(0x3F);	// 
	
	OledWriteCmd(CMD_SET_DISPLAY_ON);

	return 1;
}

void OledCls(unsigned char fillData)
{
	char buf[SPI_MAX_BUF];
	int i,j;	

	OledWriteCmd(CMD_SET_COLUMN_ADDRESS);
	OledWriteData(0x1C);
	
	OledWriteCmd(CMD_SET_ROW_ADDRESS);
	OledWriteData(0x00);

	OledWriteCmd(CMD_WRITE_RAM_CMD);
	gpio_set_value(oled.gpio_dc,1);

	for(i=0;i<SPI_MAX_BUF;i++)
		buf[i]=fillData;;
	for(j=0;j<16;j++)
		spi_write(oled.spi,buf,SPI_MAX_BUF);
}

int OledWriteCmd(unsigned char cmd)
{
	gpio_set_value(oled.gpio_dc,0);
	return WriteSpi(cmd);
}

int OledWriteData(unsigned char data)
{
	gpio_set_value(oled.gpio_dc,1);
	return WriteSpi(data);
}

int WriteSpi(unsigned char data)
{
	char buf[2];

	buf[0]=data;
	return spi_write(oled.spi,buf,1);
}

void OledSetPixel(unsigned char x,unsigned char y,unsigned int color)
{
	OledWriteCmd(CMD_SET_COLUMN_ADDRESS);
	OledWriteData(0x1C+(x>>2));
	
	OledWriteCmd(CMD_SET_ROW_ADDRESS);
	OledWriteData(y);
	
	OledWriteCmd(CMD_WRITE_RAM_CMD);
	OledWriteData(color>>8);
	OledWriteData(color);
}

static int Oled_probe(struct spi_device *spi)
{
	struct device_node *np=spi->dev.of_node;
	int i,ret;

	tt=0;

	printk(KERN_EMERG "Init Oled.\n");

	///////////////////////////////////////////////////////////////
	// Init GPIO
	///////////////////////////////////////////////////////////////
	oled.gpio_rs=of_get_named_gpio(np,"oled-rs",0);
	if(oled.gpio_rs<0)
	{
		printk("failed to find oled-rst node!\n");
		return -EINVAL;
	}
	oled.gpio_dc=of_get_named_gpio(np,"oled-dc",0);
	if(oled.gpio_dc<0)
	{
		printk("failed to find oled-rs node!\n");
		return -EINVAL;
	}
	ret=gpio_request(oled.gpio_dc,"Oled DC 1");
	if(ret)	printk(KERN_EMERG "Error: gpio_request: %d\n",oled.gpio_dc);
	ret=gpio_request(oled.gpio_rs,"Oled RS");
	if(ret)	printk(KERN_EMERG "Error: gpio_request: %d\n",oled.gpio_rs);

	gpio_direction_output(oled.gpio_dc,1);
	gpio_direction_output(oled.gpio_rs,1);
	gpio_set_value(oled.gpio_dc,0);
	gpio_set_value(oled.gpio_rs,0);

	oled.spi=spi;

	OledInit();
	OledCls(0x44);
	OledCls(0x00);
	OledSetPixel(0,0,0xF000);
	OledSetPixel(252,0,0x000F);
	OledSetPixel(0,63,0xF000);
	OledSetPixel(252,63,0x000F);
	for(i=0;i<16;i++)
	{
		OledSetPixel(127,i,0x1000*i);
		OledSetPixel(63,i,0x1000*i);
		OledSetPixel(191,i,0x1000*i);
	}
	
	/* setup your timer to call my_timer_callback */
	setup_timer(&my_timer, my_timer_callback, 0);
	/* setup timer interval to 1000 msecs */
	ret=mod_timer(&my_timer,jiffies+msecs_to_jiffies(TIMER_PERIOD));
	if(ret)	printk("Error in mod_timer\n");

	return 0;
}

static const struct spi_device_id oled_device_id[] = {
	{.name = "saak,oled-2.8",.driver_data = OLED_2_8,},
	{.name = "saak,oled-2.8-font",.driver_data = OLED_2_8_FONT,},
	{ },
};
MODULE_DEVICE_TABLE(spi,oled_device_id);

static const struct of_device_id oled_dt_ids[] = {
	{ .compatible = "saak,oled-2.8", .data = (void *) OLED_2_8, },
	{ .compatible = "saak,oled-2.8-font", .data = (void *) OLED_2_8_FONT, },
	{ },
};
MODULE_DEVICE_TABLE(of,oled_dt_ids);

static int Oled_spi_remove(struct spi_device *spi)
{
// 	struct fb_info *info = spi_get_drvdata(spi);
// 
// 	spi_set_drvdata(spi,NULL);
	/* TODO: release gpios */

	/* remove kernel timer when unloading module */
	del_timer(&my_timer);

	gpio_free(oled.gpio_dc);
	gpio_free(oled.gpio_rs);

	printk(KERN_EMERG "Close Oled.\n");
	return 0;
}

static struct spi_driver Oled_spi_driver=
{
	.driver		=
	{
		.name		= "oled",
		.bus = &spi_bus_type,
		.owner	= THIS_MODULE,
		.of_match_table = oled_dt_ids,	// this table is need for device tree usage
	},
	.id_table	= oled_device_id,
	.probe		= Oled_probe,
	.remove		= Oled_spi_remove,
};


int OledModuleInit()
{
	int ret;

	printk(KERN_EMERG "OledModuleInit\n");
	
	ret=spi_register_driver(&Oled_spi_driver);
	if(ret)
		pr_err("Oled: Unable to register SPI driver: %d\n", ret);	
// 	return spi_register_driver(&Oled_spi_driver);
	return ret;
}

void OledModuleExit()
{
	printk(KERN_EMERG "OledModuleExit\n");
	spi_unregister_driver(&Oled_spi_driver);
}

module_init(OledModuleInit);
module_exit(OledModuleExit);
