#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
//#include <poll.h>
#include "oled28.h"
using namespace std;

int main(int argc, char *argv[])
{
	cout << "Testing the Oled 2.8" << endl;

	InitGpio();
	OledInit();

	cout << "Init Oled 2.8 done..." << endl;
	
	ReleaseGpio();
	
	return 0;
}

void InitGpio()
{
}

void ReleaseGpio()
{
}

int OledInit()
{
	WriteSpi(0x01);
	
	return 1;
}

int WriteSpi(unsigned char data)
{
	char buf[MAX_BUF];

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
