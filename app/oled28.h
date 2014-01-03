#ifndef OLED28_H_
#define OLED28_H_

#define SYSFS_SPI_DIR		"/dev/spidev1.0"
#define MAX_BUF				64

int main(int argc,char *argv[]);
void InitGpio(void);
void ReleaseGpio(void);
int OledInit(void);
int WriteSpi(unsigned char);

char sysFsSpi[64];

#endif /* OLED28_H_ */
