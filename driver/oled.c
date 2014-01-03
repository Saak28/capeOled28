#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/unistd.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include "oled.h"

#define DRIVER_AUTHOR "Saak Dertadian <s.dertadian@gmail.com>"
#define DRIVER_DESC   "First Led Test Driver"
MODULE_LICENSE("GPL");

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

// 	u8 buf[10]={0x55, 1, 2, 3, 4 };
// 	spi_write(spi_device,buf,1);
}

// int InitSpi()
// {
// 	///////////////////////////////////////////////////////////////
// 	// Init SPI
// 	///////////////////////////////////////////////////////////////
// 	struct device *pdev;
// 	char buff[64];
// 	int status = 0;
// 
// 	spi_master=spi_busnum_to_master(SPI_BUS);
// 	if (!spi_master)
// 	{
// 		printk(KERN_ALERT "spi_busnum_to_master(%d) returned NULL\n",SPI_BUS);
// 		printk(KERN_ALERT "Missing modprobe omap2_mcspi?\n");
// 		return 0;
// 	}
// 	spi_device=spi_alloc_device(spi_master);
// 	if(!spi_device)
// 	{
// 		put_device(&spi_master->dev);
// 		printk(KERN_ALERT "spi_alloc_device() failed\n");
// 		return 0;
// 	}
//  
// 	/* specify a chip select line */
// 	spi_device->chip_select=SPI_BUS_CS0;
// 
// 	/* Check whether this SPI bus.cs is already claimed */
// 	snprintf(buff,sizeof(buff),"%s.%u", 
// 			dev_name(&spi_device->master->dev),
// 			spi_device->chip_select);
//  
// 	pdev=bus_find_device_by_name(spi_device->dev.bus, NULL, buff);
//  	if(pdev)
// 	{
// 		/* We are not going to use this spi_device, so free it */ 
// 		spi_dev_put(spi_device);
// 		
// 		/* 
// 		 * There is already a device configured for this bus.cs combination.
// 		 * It's okay if it's us. This happens if we previously loaded then 
//                  * unloaded our driver. 
//                  * If it is not us, we complain and fail.
// 		 */
// 		if (pdev->driver && pdev->driver->name && 
// 				strcmp(SPI_DRIVER_NAME,pdev->driver->name))
// 		{
// 			printk(KERN_ALERT  "Driver [%s] already registered for %s\n",pdev->driver->name,buff);
// 			status = -1;
// 		} 
// 	}
// 	else
// 	{
// 		spi_device->max_speed_hz=SPI_BUS_SPEED;
// 		spi_device->mode=SPI_MODE_0;
// 		spi_device->bits_per_word=8;
// 		spi_device->irq=-1;
// 		spi_device->controller_state=NULL;
// 		spi_device->controller_data=NULL;
// 		strlcpy(spi_device->modalias,"Oled",6);
// 		status=spi_add_device(spi_device);
// 		if(status<0)
// 		{	
// 			spi_dev_put(spi_device);
// 			printk(KERN_ALERT "spi_add_device() failed: %d\n",status);		
// 		}				
// 	}
// 	put_device(&spi_master->dev);
/* 
	return status;
	
}*/

int OledInit()
{
	gpio_set_value(GPIO_RS,0);
	udelay(120);
	gpio_set_value(GPIO_RS,1);

// 	OledWriteCmd(CMD_SET_COMMAND_LOCK);
// 	OledWriteData(0x12);	// Unlock Basic Commands (0x12/0x16)
// 	
// 	OledWriteCmd(CMD_SET_DISPLAY_OFF);	// Display Off (0x00/0x01)
// 	
// 	OledWriteCmd(CMD_SET_COLUMN_ADDRESS);
// 	OledWriteData(0x1C);
// 	OledWriteData(0x5B);
// 	
// 	OledWriteCmd(CMD_SET_ROW_ADDRESS);
// 	OledWriteData(0x00);
// 	OledWriteData(0x3F);
// 	
// 	OledWriteCmd(CMD_SET_DISPLAY_CLOCK);
// 	OledWriteData(0x91);	// Set Clock as 80 Frames/Sec
// 	
// 	OledWriteCmd(CMD_SET_MULTIPLEX_RATIO);
// 	OledWriteData(0x3F);	// 1/64 Duty (0x0F~0x3F)
// 	
// 	OledWriteCmd(CMD_SET_DISPLAY_OFFSET);
// 	OledWriteData(0x00);	// Shift Mapping RAM Counter (0x00~0x3F)
// 	
// 	OledWriteCmd(CMD_SET_DISPLAY_START_LINE);
// 	OledWriteData(0x00);	// Set Mapping RAM Display Start Line (0x00~0x7F)
// 	
// 	OledWriteCmd(CMD_SET_REMAP);
// 	OledWriteData(0x14);	// 0x14
// 	OledWriteData(0x11);	// Enable dual COM mode
// 	
// 	OledWriteCmd(CMD_SET_GPIO);
// 	OledWriteData(0x00);	// Disable GPIO input pin
// 	
// 	OledWriteCmd(CMD_FUNCTION_SELECTION);
// 	OledWriteData(0x01);	// Enable internal VDD regulator
// 
// 	OledWriteCmd(CMD_SET_DISPLAY_ENHANCEMENT_A);
// 	OledWriteData(0xA0);	// Set_Display_Enhancement_A(0xA0,0xFD);
// 	OledWriteData(0xFD);
// 	
// 	OledWriteCmd(CMD_SET_CONTRAST_CONTROL);
// 	OledWriteData(0x9F);	// Set Segment Output Current
// 	
// 	OledWriteCmd(CMD_MASTER_CONTRAST_CONTROL);
// 	OledWriteData(0x0F);	// Set Scale Factor of Segment Output Current Control 
// 	
// 	OledWriteCmd(CMD_SELECT_DEFAULT);	// set default linear gray scale table
// 	OledWriteCmd(CMD_ENABLE_GRAYSCALE_TABLE);
// 	
// 	OledWriteCmd(CMD_SET_PHASE_LENGTH);
// 	OledWriteData(0xE2);	// Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
// 
// 	OledWriteCmd(CMD_SET_DISPLAY_ENHANCEMENT_B);
// 	OledWriteData(0x20);	// Set_Display_Enhancement_B(0x20);
// 	
// 	OledWriteCmd(CMD_SET_PRECHARGE_VOLTAGE);
// 	OledWriteData(0x1F);	// Set Pre‐Charge Voltage Level as 0.60*VCC
// 	
// 	OledWriteCmd(CMD_SET_SECOND_PRECHARGE_PERIOD);
// 	OledWriteData(0x08);	// Set Second Pre‐Charge Period as 8 Clocks 
// 	
// 	OledWriteCmd(CMD_SET_VCOMH_VOLTAGE);
// 	OledWriteData(0x07);	// Set Common Pins Deselect Voltage Level as 0.86*VCC
// 	
// 	OledWriteCmd(CMD_DISPLAY_MODE_NORMAL);	// Normal Display Mode (0x00/0x01/0x02/0x03)
// 	
// 	OledWriteCmd(CMD_EXIT_PARTIAL_DISPLAY);
// // 	OledWriteCmd(CMD_ENABLE_PARTIAL_DISPLAY);
// // 	OledWriteData(0x00);	// Disable Partial Display
// // 	OledWriteData(0x3F);	// 
// 	
// 	OledWriteCmd(CMD_SET_DISPLAY_ON);

	return 1;
}
// 
// void OledCls(unsigned char fillData)
// {
// 	OledWriteCmd(CMD_SET_COLUMN_ADDRESS);
// 	OledWriteData(0x1C);
// 	
// 	OledWriteCmd(CMD_SET_ROW_ADDRESS);
// 	OledWriteData(0x00);
// 
// 	OledWriteCmd(CMD_WRITE_RAM_CMD);
// 	gpio_set_value(GPIO_DC,HIGH);
// 
// 	char buf[SPI_MAX_BUF];
// 	int spiFile;	
// 	if(!OpenSpiDevice(&spiFile))
// 		return;
// 
// 	for(int i=0;i<SPI_MAX_BUF;i++)
// 		buf[i]=fillData;;
// 	for(int j=0;j<8;j++)
// 		write(spiFile,buf,SPI_MAX_BUF);
// 	CloseSpiDevice(&spiFile);
// }
// 
// int OledWriteCmd(unsigned char cmd)
// {
// 	gpio_set_value(GPIO_DC,LOW);
// 	return WriteSpi(cmd);
// }
// 
// int OledWriteData(unsigned char data)
// {
// 	gpio_set_value(GPIO_DC,HIGH);
// 	return WriteSpi(data);
// }
// 
// int WriteSpi(unsigned char data)
// {
// 	char buf[SPI_MAX_BUF];
// 
// 	int fd=open(SYSFS_SPI_DIR,O_WRONLY);
// 	if(fd<0)
// 	{
// 		perror("WriteSpi");
// 		return 0;
// 	}
// 	buf[0]=data;
// 	write(fd,buf,1);
// 	close(fd);
// 	return 1;
// }
// 
// int OpenSpiDevice(int *spiFile)
// {
// 	*spiFile=open(SYSFS_SPI_DIR,O_WRONLY);
// 	if(spiFile<0)
// 	{
// 		perror("WriteSpi");
// 		return 0;
// 	}
// 	return 1;
// }
// 
// void CloseSpiDevice(int *spiFile)
// {
// 	close(*spiFile);
// }
// 
// void OledSetPixel(unsigned char x,unsigned char y,unsigned int color)
// {
// 	OledWriteCmd(CMD_SET_COLUMN_ADDRESS);
// 	OledWriteData(0x1C+(x>>2));
// 	
// 	OledWriteCmd(CMD_SET_ROW_ADDRESS);
// 	OledWriteData(y);
// 	
// 	OledWriteCmd(CMD_WRITE_RAM_CMD);
// 	OledWriteData(color>>8);
// 	OledWriteData(color);
// }

static const struct spi_device_id oled_device_id[] = {
	{
		.name = "oled-2.8",
		.driver_data = OLED_2_8,
	},
	{
		.name = "oled-2.8-font",
		.driver_data = OLED_2_8_FONT,
	},
	{
	}
};
MODULE_DEVICE_TABLE(spi,oled_device_id);

static const struct of_device_id oled_dt_ids[] = {
	{ .compatible = "saak,oled-2.8", .data = (void *) OLED_2_8, },
	{ .compatible = "saak,oled-2.8-font", .data = (void *) OLED_2_8_FONT, },
	{ 0,0, },
};
MODULE_DEVICE_TABLE(of,oled_dt_ids);

static int Oled_probe(struct spi_device *spi)
{
	int ret;

	tt=0;

	printk(KERN_EMERG "Init Oled.\n");

	///////////////////////////////////////////////////////////////
	// Init GPIO
	///////////////////////////////////////////////////////////////
	ret=gpio_request(GPIO_DC,"Oled DC 1");
	if(ret)	printk(KERN_EMERG "Error: gpio_request: %d\n",GPIO_DC);
	ret=gpio_request(GPIO_RS,"Oled RS");
	if(ret)	printk(KERN_EMERG "Error: gpio_request: %d\n",GPIO_RS);

	gpio_direction_output(GPIO_DC,1);
	gpio_direction_output(GPIO_RS,1);
	gpio_set_value(GPIO_DC,0);
	gpio_set_value(GPIO_RS,0);

// 	if(!InitSpi())
// 		return 0;
	OledInit();
	
	/* setup your timer to call my_timer_callback */
	setup_timer(&my_timer, my_timer_callback, 0);
	/* setup timer interval to 1000 msecs */
	ret=mod_timer(&my_timer,jiffies+msecs_to_jiffies(TIMER_PERIOD));
	if(ret)	printk("Error in mod_timer\n");

	return 0;
}

// static int __devexit Oled_spi_remove(struct spi_device *spi)
static int Oled_spi_remove(struct spi_device *spi)
{
// 	struct fb_info *info = spi_get_drvdata(spi);
// 
// 	spi_set_drvdata(spi,NULL);
	/* TODO: release gpios */

	/* remove kernel timer when unloading module */
	del_timer(&my_timer);

	gpio_free(GPIO_DC);
	gpio_free(GPIO_RS);

	printk(KERN_EMERG "Close Oled.\n");
	return 0;
}

static struct spi_driver Oled_spi_driver=
{
	.id_table = oled_device_id,
	.driver		=
	{
		.name		= "oled",
		.owner	= THIS_MODULE,
		.of_match_table = oled_dt_ids,
	},
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
