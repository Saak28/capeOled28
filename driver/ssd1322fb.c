/*
 * linux/drivers/video/ssd1322fb.c -- FB driver for SSD1322 OLED controller
 * Layout is based on SSD1322fb.c from James Simmons and Geert Uytterhoeven.
 * by Saak Dertadian
 * Copyright (C) 2014, Saak Dertadian <s.dertadian@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file COPYING in the main directory of this archive for
 * more details.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>

#include "ssd1322fb.h"

static struct ssd1322_function ssd1322_cfg_script[]=
{
	{ SSD1322_START, SSD1322_START},

	{ SSD1322_CMD, CMD_SET_COMMAND_LOCK},			// Unlock Basic Commands (0x12/0x16)
	{ SSD1322_DATA, 0x12},

	{ SSD1322_CMD, CMD_SET_DISPLAY_OFF},			// Display Off (0x00/0x01)

	{ SSD1322_CMD, CMD_SET_COLUMN_ADDRESS},
	{ SSD1322_DATA, 0x1C},
	{ SSD1322_DATA, 0x5B},

	{ SSD1322_CMD, CMD_SET_ROW_ADDRESS},
	{ SSD1322_DATA, 0x00},
	{ SSD1322_DATA, 0x5B},

	{ SSD1322_CMD, CMD_SET_DISPLAY_CLOCK},			// Set Clock as 80 Frames/Sec
	{ SSD1322_DATA, 0x91},

	{ SSD1322_CMD, CMD_SET_MULTIPLEX_RATIO},		// 1/64 Duty (0x0F~0x3F)
	{ SSD1322_DATA, 0x3F},

	{ SSD1322_CMD, CMD_SET_DISPLAY_OFFSET},			// Shift Mapping RAM Counter (0x00~0x3F)
	{ SSD1322_DATA, 0x00},

	{ SSD1322_CMD, CMD_SET_DISPLAY_START_LINE},
	{ SSD1322_DATA, 0x00},							// Set Mapping RAM Display Start Line (0x00~0x7F)

	{ SSD1322_CMD, CMD_SET_REMAP},
	{ SSD1322_DATA, 0x14},
	{ SSD1322_DATA, 0x11},							// Enable dual COM mode

	{ SSD1322_CMD, CMD_SET_GPIO},					// Disable GPIO input pin
	{ SSD1322_DATA, 0x00},

	{ SSD1322_CMD, CMD_FUNCTION_SELECTION},			// Enable internal VDD regulator
	{ SSD1322_DATA, 0x01},

	{ SSD1322_CMD, CMD_SET_DISPLAY_ENHANCEMENT_A},	// Set_Display_Enhancement_A(0xA0,0xFD);
	{ SSD1322_DATA, 0xA0},
	{ SSD1322_DATA, 0xFD},

	{ SSD1322_CMD, CMD_SET_CONTRAST_CONTROL},		// Set Segment Output Current
	{ SSD1322_DATA, 0x9F},

	{ SSD1322_CMD, CMD_MASTER_CONTRAST_CONTROL},	// Set Scale Factor of Segment Output Current Control
	{ SSD1322_DATA, 0x0F},

	{ SSD1322_CMD, CMD_SELECT_DEFAULT},				// set default linear gray scale table

	{ SSD1322_CMD, CMD_ENABLE_GRAYSCALE_TABLE},

	{ SSD1322_CMD, CMD_SET_PHASE_LENGTH},			// Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
	{ SSD1322_DATA, 0xE2},

	{ SSD1322_CMD, CMD_SET_DISPLAY_ENHANCEMENT_B},	// Set_Display_Enhancement_B(0x20);
	{ SSD1322_DATA, 0x20},

	{ SSD1322_CMD, CMD_SET_PRECHARGE_VOLTAGE},		// Set Pre‐Charge Voltage Level as 0.60*VCC
	{ SSD1322_DATA, 0x1F},

	{ SSD1322_CMD, CMD_SET_SECOND_PRECHARGE_PERIOD},	// Set Second Pre‐Charge Period as 8 Clocks
	{ SSD1322_DATA, 0x08},

	{ SSD1322_CMD, CMD_SET_VCOMH_VOLTAGE},			// Set Common Pins Deselect Voltage Level as 0.86*VCC
	{ SSD1322_DATA, 0x07},

	{ SSD1322_CMD, CMD_DISPLAY_MODE_NORMAL},		// Normal Display Mode (0x00/0x01/0x02/0x03)

	{ SSD1322_CMD, CMD_EXIT_PARTIAL_DISPLAY},
	{ SSD1322_CMD, CMD_SET_DISPLAY_ON},
	{ SSD1322_END, SSD1322_END},
};

static struct fb_fix_screeninfo ssd1322fb_fix=
{
	.id =		"SSD1322",
	.type =			FB_TYPE_PACKED_PIXELS,
	.visual =		FB_VISUAL_DIRECTCOLOR,
	.xpanstep =		0,
	.ypanstep =		0,
	.ywrapstep =	0,
	.line_length =	WIDTH*BPP/8,
	.accel =		FB_ACCEL_NONE,
};

static struct fb_var_screeninfo ssd1322fb_var=
{
	.xres =			WIDTH,
	.yres =			HEIGHT,
	.xres_virtual =	WIDTH,
	.yres_virtual =	HEIGHT,
	.bits_per_pixel=BPP,
	.grayscale=		16,
	.nonstd	=		0,
};

static int ssd1322_write(struct ssd1322fb_par *par, u8 data)
{
	par->buf[0]=data;

	return spi_write(par->spi,par->buf,1);
}

static void ssd1322_write_data(struct ssd1322fb_par *par,u8 data)
{
	int ret = 0;

	/* Set data mode */
	gpio_set_value(par->dc,1);

	ret=ssd1322_write(par,data);
	if(ret<0)
		pr_err("%s: write data %02x failed with status %d\n",par->info->fix.id,data,ret);
}

static int ssd1322_write_data_buf(struct ssd1322fb_par *par,u8 *txbuf, int size)
{
//	printk(KERN_EMERG "==> %02X %02X %02X %02X %02X %02X %02X %02X\n",txbuf[0],txbuf[1],txbuf[2],txbuf[3],txbuf[4],txbuf[5],txbuf[6],txbuf[7]);
	gpio_set_value(par->dc,1);	// Set data mode
	return spi_write(par->spi,txbuf,size);	// Write entire buffer
}

static void ssd1322_write_cmd(struct ssd1322fb_par *par,u8 data)
{
	int ret;

	gpio_set_value(par->dc,0);	// Set command mode

	ret=ssd1322_write(par,data);
	if(ret<0)
		pr_err("%s: write command %02x failed with status %d\n",par->info->fix.id,data,ret);
}

static void ssd1322_run_cfg_script(struct ssd1322fb_par *par)
{
	int i=0;
	int end_script=0;

//	printk(KERN_EMERG "----> ssd1322_run_cfg_script <----\n");
	do
	{
		switch (ssd1322_cfg_script[i].cmd)
		{
		case SSD1322_START:
			break;
		case SSD1322_CMD:
			ssd1322_write_cmd(par,ssd1322_cfg_script[i].data&0xff);
			break;
		case SSD1322_DATA:
			ssd1322_write_data(par,ssd1322_cfg_script[i].data&0xff);
			break;
		case SSD1322_DELAY:
			mdelay(ssd1322_cfg_script[i].data);
			break;
		case SSD1322_END:
			end_script=1;
		}
		i++;
	}while(!end_script);
}

static void ssd1322Cls(struct ssd1322fb_par *par,unsigned char fillData)
{
	char buf[512];
	int i,j;

	ssd1322_write_cmd(par,CMD_SET_COLUMN_ADDRESS);
	ssd1322_write_data(par,0x1C);

	ssd1322_write_cmd(par,CMD_SET_ROW_ADDRESS);
	ssd1322_write_data(par,0x00);

	ssd1322_write_cmd(par,CMD_WRITE_RAM_CMD);
	gpio_set_value(par->dc,1);	// Set data mode

	for(i=0;i<512;i++)
		buf[i]=fillData;;
	for(j=0;j<16;j++)
		spi_write(par->spi,buf,512);
}

static void ssd1322_set_addr_win(struct ssd1322fb_par *par,int xs,int ys,int xe,int ye)
{
//	printk(KERN_EMERG "----> ssd1322_set_addr_win <----%d-%d <> %d-%d, offX:%d - offY:%d\n",xs,xe,ys,ye,par->xoff,par->yoff);
	ssd1322_write_cmd(par,CMD_SET_COLUMN_ADDRESS);
	ssd1322_write_data(par,0x1C+(xs+par->xoff)/BPP);
	ssd1322_write_data(par,0x1C+(xe+par->xoff)/BPP);
//	ssd1322_write_data(par,0x1C);
//	ssd1322_write_data(par,0x5B);

	ssd1322_write_cmd(par,CMD_SET_ROW_ADDRESS);
	ssd1322_write_data(par,ys+par->yoff);
	ssd1322_write_data(par,ye+par->yoff);
//	ssd1322_write_data(par,0x00);
//	ssd1322_write_data(par,0x3F);
}

static void ssd1322_reset(struct ssd1322fb_par *par)
{
	/* Reset controller */
	gpio_set_value(par->rst,0);
	udelay(100);
	gpio_set_value(par->rst,1);
	mdelay(200);
}

static void ssd1322fb_update_display(struct ssd1322fb_par *par)
{
	int ret=0;
	u16 *vmem;
	int i,k,c;
	static int kk=0x8000;
	static int div=0;

//	printk(KERN_EMERG "----> ssd1322fb_update_display <----\n");
	u16 *vmem16=(u16 *)par->info->screen_base;
	vmem=par->ssbuf;
	for (i=0;i<WIDTH*HEIGHT*BPP/8/2;i++)
	{
//		vmem[i]=(vmem16[i]&0xFFFF);
//		vmem[i]=(vmem16[i]);

		vmem[i]=0x0000;
		if(vmem16[i]&0x2000)	vmem[i]|=0xF000;
		if(vmem16[i]&0x4000)	vmem[i]|=0x0F00;
		if(vmem16[i]&0x0200)	vmem[i]|=0x00F0;
		if(vmem16[i]&0x0200)	vmem[i]|=0x000F;
//		if(vmem16[i]&kk)		vmem[i]|=0x000F;

//		if(vmem16[i]&kk)
//			vmem[i]=0xFFFF;
//		else
//			vmem[i]=0x0000;
	}
//	if(++div>=4)
//	{
//		div=0;
		kk>>=1;
//	}
	if(!kk)
	{
		kk=0x8000;
//		printk(KERN_EMERG "kk=0x8000\n");
	}
	//		vmem[i]=swab16(vmem16[i]);

	mutex_lock(&(par->io_lock));

	/* Set row/column data window */
	ssd1322_set_addr_win(par,0,0,WIDTH-1,HEIGHT-1);

	/* Internal RAM write command */
	ssd1322_write_cmd(par,CMD_WRITE_RAM_CMD);

//	for(i=0;i<4096;i++)
//		vmem[i]=0x4444;
	/* Blast framebuffer to SSD1322 internal display RAM */
//	printk(KERN_EMERG "%04X %04X %04X %04X\n",vmem[0],vmem[1],vmem[2],vmem[3]);

	ret=ssd1322_write_data_buf(par,(u8 *)vmem,WIDTH*HEIGHT*BPP/8);
	if(ret<0)
		pr_err("%s: spi_write failed to update display buffer\n",par->info->fix.id);

	mutex_unlock(&(par->io_lock));
}

static int ssd1322fb_init_display(struct ssd1322fb_par *par)
{
	int i;
	int ret=0;

	/* Request GPIOs and initialize to default values */
	ret=gpio_request_one(par->rst,GPIOF_OUT_INIT_HIGH,"SSD1322 Reset Pin");
	if(ret<0)
	{
		pr_err("%s: failed to claim reset pin\n",par->info->fix.id);
		goto out;
	}
	ret=gpio_request_one(par->dc, GPIOF_OUT_INIT_LOW,"SSD1322 Data/Command Pin");
	if(ret<0)
	{
		pr_err("%s: failed to claim data/command pin\n",par->info->fix.id);
		goto out;
	}
	ssd1322_reset(par);
	ssd1322_run_cfg_script(par);

	// Splash Screen
	for(i=0x00;i<0x100;i+=0x11)
	{
		ssd1322Cls(par,i);
		mdelay(10);
	}
	for(i=0xFF;i>=0;i-=0x11)
	{
		ssd1322Cls(par,i);
		mdelay(10);
	}
out:
	return ret;
}

static void ssd1322fb_deferred_io(struct fb_info *info,struct list_head *pagelist)
{
//	printk(KERN_EMERG "----> ssd1322fb_deferred_io <----\n");
	ssd1322fb_update_display(info->par);
}

static void ssd1322fb_update_display_deferred(struct fb_info *info)
{
	struct fb_deferred_io *fbdefio=info->fbdefio;
//	printk(KERN_EMERG "----> ssd1322fb_update_display_deferred <----\n");
	schedule_delayed_work(&info->deferred_work,fbdefio->delay);
}

static void ssd1322fb_fillrect(struct fb_info *info,const struct fb_fillrect *rect)
{
//	printk(KERN_EMERG "----> ssd1322fb_fillrect <----\n");
	sys_fillrect(info,rect);
	ssd1322fb_update_display_deferred(info);
}

static void ssd1322fb_copyarea(struct fb_info *info,const struct fb_copyarea *area)
{
//	printk(KERN_EMERG "----> ssd1322fb_copyarea <----\n");
	sys_copyarea(info, area);
	ssd1322fb_update_display_deferred(info);
}

static void ssd1322fb_imageblit(struct fb_info *info,const struct fb_image *image)
{
//	printk(KERN_EMERG "----> ssd1322fb_imageblit <----\n");
	sys_imageblit(info,image);
	ssd1322fb_update_display_deferred(info);
}

static ssize_t ssd1322fb_write(struct fb_info *info,const char __user *buf,size_t count,loff_t *ppos)
{
	unsigned long p=*ppos;
	void *dst;
	int err=0;
	unsigned long total_size;

//	printk(KERN_EMERG "----> ssd1322fb_write <---- %d %d %d %d\n",count,buf[0],buf[1],buf[2]);
	if(info->state!=FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size=info->fix.smem_len;
	if(p>total_size)
		return -EFBIG;

	if(count>total_size)
	{
		err=-EFBIG;
		count=total_size;
	}

	if(count+p>total_size)
	{
		if (!err)
			err=-ENOSPC;
		count=total_size-p;
	}

	dst=(void __force *)(info->screen_base+p);

	if(copy_from_user(dst,buf,count))
		err=-EFAULT;
	if(!err)
		*ppos+=count;
	ssd1322fb_update_display_deferred(info);
	return (err)?err:count;
}

static int ssd1322fb_setcolreg(unsigned regno,unsigned red,unsigned green,
							   unsigned blue,unsigned transp,
							   struct fb_info *info)
{
//	printk(KERN_EMERG "----> ssd1322fb_setcolreg <----%d - R:%04X, G:%04X, B:%04X\n",regno,red,green,blue);
	if(regno>=MAX_PALETTE)
		return -EINVAL;

	/* RGB565 */
	((u32*)(info->pseudo_palette))[regno]=
			((red & 0xf800) |
			((green & 0xfc00) >> 5) |
			((blue & 0xf800) >> 11));
//	((u32*)(info->pseudo_palette))[regno]=
//			((red & 0xF000) |
//			((green & 0xF000) >> 4) |
//			((blue & 0xF000) >> 8));
	return 0;
}

static struct fb_ops ssd1322fb_ops=
{
	.owner			= THIS_MODULE,
	.fb_read		= fb_sys_read,
	.fb_write		= ssd1322fb_write,
	.fb_fillrect	= ssd1322fb_fillrect,
	.fb_copyarea	= ssd1322fb_copyarea,
	.fb_imageblit	= ssd1322fb_imageblit,
	.fb_setcolreg	= ssd1322fb_setcolreg,
};

static struct fb_deferred_io ssd1322fb_defio=
{
	.delay			= HZ/20,
	.deferred_io	= ssd1322fb_deferred_io,
};

static int ssd1322fb_probe(struct spi_device *spi)
{
	int vmem_size=WIDTH*HEIGHT*BPP/8;
	u8 *vmem;
	struct fb_info *info;
	struct ssd1322fb_par *par;
	int retval=-ENOMEM;
	struct device_node *np=spi->dev.of_node;
//	const struct spi_device_id *spi_id=spi_get_device_id(spi);
	struct pinctrl *pinctrl;

//	if(!spi_id)
//	{
//		dev_err(&spi->dev,"device id not supported! %d\n",(int)spi_id);
//		return -EINVAL;
//	}
	pinctrl=devm_pinctrl_get_select_default(&spi->dev);
	if(IS_ERR(pinctrl))
		dev_warn(&spi->dev,"pins are not configured from the driver\n");

#ifdef __LITTLE_ENDIAN
	vmem=(u8 *)vmalloc(vmem_size);
#else
	vmem=(u8 *)kmalloc(vmem_size,GFP_KERNEL);
#endif
	if(!vmem)
		return retval;

	info=framebuffer_alloc(sizeof(struct ssd1322fb_par),&spi->dev);
	if(!info)
		goto fballoc_fail;

	info->pseudo_palette=kmalloc(sizeof(u32)*MAX_PALETTE,GFP_KERNEL);
	if(!info->pseudo_palette)
		goto palette_fail;

	info->screen_base=(u8 __force __iomem *)vmem;
	info->fbops=&ssd1322fb_ops;
	info->fix=ssd1322fb_fix;
	info->fix.smem_len=vmem_size;
	info->var=ssd1322fb_var;
	/* Choose any packed pixel format as long as it's RGB565 */
	info->var.red.offset = 11;
	info->var.red.length = 5;
	info->var.green.offset = 5;
	info->var.green.length = 6;
	info->var.blue.offset = 0;
	info->var.blue.length = 5;
	info->var.transp.offset = 0;
	info->var.transp.length = 0;
	info->flags=FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;

	info->fbdefio = &ssd1322fb_defio;
	fb_deferred_io_init(info);

	retval=fb_alloc_cmap(&info->cmap, MAX_PALETTE,0);
	if(retval < 0)
		goto cmap_fail;
	info->cmap.len=MAX_PALETTE;

	par=info->par;
	par->info=info;
	par->spi=spi;

	mutex_init(&par->io_lock);

	par->xoff = 0;
	par->yoff = 0;

	/* TODO: fix all exit paths for cleanup */
	par->rst=of_get_named_gpio(np, "oled-rs", 0);
	if(par->rst < 0)
	{
		printk(KERN_EMERG "failed to find SSD1322-rst node!\n");
		return -EINVAL;
	}
	gpio_rst=par->rst;

	par->dc=of_get_named_gpio(np, "oled-dc", 0);
	if(par->dc<0)
	{
		printk(KERN_EMERG "failed to find SSD1322-dc node!\n");
		return -EINVAL;
	}
	gpio_dc=par->dc;
	printk(KERN_EMERG "DC=%d, RS=%d\n",gpio_dc,gpio_rst);

//	gpio_rst=60;
//	gpio_dc=49;

	par->buf=kmalloc(1,GFP_KERNEL);
	if(!par->buf)
	{
		retval=-ENOMEM;
		goto buf_fail;
	}

	/* Allocated swapped shadow buffer */
	par->ssbuf=kmalloc(vmem_size, GFP_KERNEL);
	if(!par->ssbuf)
	{
		retval=-ENOMEM;
		goto ssbuf_fail;
	}

	retval=ssd1322fb_init_display(par);
	if(retval<0)
		goto init_fail;

	retval=register_framebuffer(info);
	if(retval<0)
		goto fbreg_fail;

	spi_set_drvdata(spi,info);

	printk(KERN_INFO "fb%d: %s frame buffer device,\n\tusing %d KiB of video memory\n",info->node,info->fix.id,vmem_size);

	return 0;

	spi_set_drvdata(spi,NULL);

fbreg_fail:
	/* TODO: release gpios on fail */
	/* TODO: and unwind everything in init */

init_fail:
	kfree(par->ssbuf);

ssbuf_fail:
	kfree(par->buf);

buf_fail:
	fb_dealloc_cmap(&info->cmap);

cmap_fail:
	kfree(info->pseudo_palette);

palette_fail:
	framebuffer_release(info);

fballoc_fail:
	vfree(vmem);
//	kfree(vmem);

	return retval;
}

static int ssd1322fb_remove(struct spi_device *spi)
{
	struct fb_info *info=spi_get_drvdata(spi);

	spi_set_drvdata(spi,NULL);

	if(info)
	{
		unregister_framebuffer(info);
		fb_dealloc_cmap(&info->cmap);
		kfree(info->pseudo_palette);
		vfree(info->screen_base);
		framebuffer_release(info);
	}

	/* TODO: release gpios */
//	printk(KERN_EMERG "ssd1322fb_remove: rst=%d dc=%d",gpio_rst,gpio_dc);
	gpio_free(gpio_rst);
	gpio_free(gpio_dc);

	return 0;
}

static const struct spi_device_id ssd1322fb_device_id[] = {
	{.name = "saak,oled-2.8",.driver_data = OLED_2_8,},
	{.name = "saak,oled-2.8-font",.driver_data = OLED_2_8_FONT,},
	{ },
};
MODULE_DEVICE_TABLE(spi,ssd1322fb_device_id);

static const struct of_device_id ssd1322fb_dt_ids[] = {
	{ .compatible = "saak,oled-2.8", .data = (void *) OLED_2_8, },
	{ .compatible = "saak,oled-2.8-font", .data = (void *) OLED_2_8_FONT, },
	{ },
};
MODULE_DEVICE_TABLE(of,ssd1322fb_dt_ids);

static struct spi_driver ssd1322fb_driver=
{
	.driver=
	{
		.name="ssd1322",
		.bus=&spi_bus_type,
		.owner =THIS_MODULE,
		.of_match_table=ssd1322fb_dt_ids,
	},
	.id_table=ssd1322fb_device_id,
	.probe=ssd1322fb_probe,
	.remove=ssd1322fb_remove,
};

static int __init ssd1322fb_init(void)
{
	printk(KERN_EMERG "================================\n");
	printk(KERN_EMERG "SSD1322Init\n");
	return spi_register_driver(&ssd1322fb_driver);
}

static void __exit ssd1322fb_exit(void)
{
	printk(KERN_EMERG "SSD1322Exit\n");
	printk(KERN_EMERG "================================\n");
	spi_unregister_driver(&ssd1322fb_driver);
}

/* ------------------------------------------------------------------------- */

module_init(ssd1322fb_init);
module_exit(ssd1322fb_exit);

MODULE_DESCRIPTION("FB driver for SSD1322 display controller");
MODULE_AUTHOR("Saak Dertadian <s.dertadian@gmail.com>");
MODULE_LICENSE("GPL");
