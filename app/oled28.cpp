#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "SimpleGPIO.h"
#include "oled28.h"
using namespace std;

unsigned int GPIO_DC=49;
unsigned int GPIO_RS=60;

int main(int argc, char *argv[])
{
	cout << "Testing the Oled 2.8" << endl;

	InitGpio();
	OledInit();
	OledCls(0x44);
	OledCls(0x00);
// 	OledCls(0xFF);
// 	usleep(1000000);
// 	OledWriteCmd(CMD_DISPLAY_MODE_INVERSE);	// Normal Display Mode (0x00/0x01/0x02/0x03)
// 	OledWriteCmd(CMD_DISPLAY_MODE_ALL_ON);	// Normal Display Mode (0x00/0x01/0x02/0x03)
	
	OledSetPixel(0,0,0xF000);
	OledSetPixel(252,0,0x000F);
	OledSetPixel(0,63,0xF000);
	OledSetPixel(252,63,0x000F);

	for(int i=0;i<16;i++)
		OledSetPixel(127,i,0x1000*i);

// 	OledSetPixel(4,0,0xF0F0);
// 	OledSetPixel(4,1,0x0F0F);
// 	OledSetPixel(0,63,0x000F);


	cout << "Init Oled 2.8 done..." << endl;
	
	ReleaseGpio();
	
	return 0;
}

void InitGpio()
{
	gpio_export(GPIO_DC);
	gpio_export(GPIO_RS);

	gpio_set_dir(GPIO_DC,OUTPUT_PIN);
	gpio_set_dir(GPIO_RS,OUTPUT_PIN);

	gpio_set_value(GPIO_DC,HIGH);
	gpio_set_value(GPIO_RS,HIGH);
}

void ReleaseGpio()
{
	gpio_unexport(GPIO_DC);
	gpio_unexport(GPIO_RS);
}

int OledInit()
{
	gpio_set_value(GPIO_RS,LOW);
	usleep(120);
	gpio_set_value(GPIO_RS,HIGH);

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
	OledWriteCmd(CMD_SET_COLUMN_ADDRESS);
	OledWriteData(0x1C);
	
	OledWriteCmd(CMD_SET_ROW_ADDRESS);
	OledWriteData(0x00);

	OledWriteCmd(CMD_WRITE_RAM_CMD);
	gpio_set_value(GPIO_DC,HIGH);

	char buf[SPI_MAX_BUF];
	int spiFile;	
	if(!OpenSpiDevice(&spiFile))
		return;

	for(int i=0;i<SPI_MAX_BUF;i++)
		buf[i]=fillData;;
	for(int j=0;j<8;j++)
		write(spiFile,buf,SPI_MAX_BUF);
	CloseSpiDevice(&spiFile);
}

int OledWriteCmd(unsigned char cmd)
{
	gpio_set_value(GPIO_DC,LOW);
	return WriteSpi(cmd);
}

int OledWriteData(unsigned char data)
{
	gpio_set_value(GPIO_DC,HIGH);
	return WriteSpi(data);
}

int WriteSpi(unsigned char data)
{
	char buf[SPI_MAX_BUF];

	int fd=open(SYSFS_SPI_DIR,O_WRONLY);
	if(fd<0)
	{
		perror("WriteSpi");
		return 0;
	}
	buf[0]=data;
	write(fd,buf,1);
	close(fd);
	return 1;
}

int OpenSpiDevice(int *spiFile)
{
	*spiFile=open(SYSFS_SPI_DIR,O_WRONLY);
	if(spiFile<0)
	{
		perror("WriteSpi");
		return 0;
	}
	return 1;
}

void CloseSpiDevice(int *spiFile)
{
	close(*spiFile);
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
