#ifndef OLED28_H_
#define OLED28_H_

#define SYSFS_SPI_DIR		"/dev/spidev1.0"
#define SPI_MAX_BUF				1024

///////////////////////////////////////////////////////
// SSD1322 COMMANDS
///////////////////////////////////////////////////////
#define CMD_ENABLE_GRAYSCALE_TABLE					0x00
#define CMD_SET_COLUMN_ADDRESS						0x15
#define CMD_WRITE_RAM_CMD								0x5C
#define CMD_READ_RAM_CMD								0x5D
#define CMD_SET_ROW_ADDRESS							0x75
#define CMD_SET_REMAP									0xA0
#define CMD_SET_DISPLAY_START_LINE					0xA1
#define CMD_SET_DISPLAY_OFFSET						0xA2
#define CMD_DISPLAY_MODE_ALL_OFF						0xA4
#define CMD_DISPLAY_MODE_ALL_ON						0xA5
#define CMD_DISPLAY_MODE_NORMAL						0xA6
#define CMD_DISPLAY_MODE_INVERSE						0xA7
#define CMD_ENABLE_PARTIAL_DISPLAY					0xA8
#define CMD_EXIT_PARTIAL_DISPLAY						0xA9
#define CMD_FUNCTION_SELECTION						0xAB
#define CMD_SET_DISPLAY_OFF							0xAE
#define CMD_SET_DISPLAY_ON								0xAF
#define CMD_SET_PHASE_LENGTH							0xB1
#define CMD_SET_DISPLAY_CLOCK							0xB3
#define CMD_SET_GPIO										0xB5
#define CMD_SET_SECOND_PRECHARGE_PERIOD			0xB6
#define CMD_SET_GRAYSCALE_TABLE						0xB8
#define CMD_SELECT_DEFAULT								0xB9
#define CMD_SET_PRECHARGE_VOLTAGE					0xBB
#define CMD_SET_VCOMH_VOLTAGE							0xBE
#define CMD_SET_CONTRAST_CONTROL						0xC1
#define CMD_MASTER_CONTRAST_CONTROL					0xC7
#define CMD_SET_MULTIPLEX_RATIO						0xCA
#define CMD_SET_COMMAND_LOCK							0xFD
#define CMD_SET_DISPLAY_ENHANCEMENT_A				0xB4
#define CMD_SET_DISPLAY_ENHANCEMENT_B				0xD1

int main(int argc,char *argv[]);
void InitGpio(void);
void ReleaseGpio(void);
int OledInit(void);
void OledCls(unsigned char);
int OledWriteCmd(unsigned char);
int OledWriteData(unsigned char);
int WriteSpi(unsigned char);
int OpenSpiDevice(int*);
void CloseSpiDevice(int*);
void OledSetPixel(unsigned char,unsigned char,unsigned int);

char sysFsSpi[64];

#endif /* OLED28_H_ */
