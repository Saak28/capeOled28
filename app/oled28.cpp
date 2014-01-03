#include <iostream>
#include <string>
#include <unistd.h>
#include "lcd2.h"
#include "SimpleGPIO.h"
#include "ILI9341.h"
using namespace std;

int main(int argc, char *argv[])
{
	cout << "Testing the LCD 2.4'" << endl;

	InitGpio();
	cout << "Init Lcd..." << endl;
	LcdInit();
	LcdTest();
	cout << "Init Lcd done..." << endl;
	usleep(2000000);
	cout << "Init Lcd done..." << endl;
	
	ReleaseGpio();
	
	return 0;
}

void InitGpio()
{
	gpio_export(GPIO_D0);
	gpio_export(GPIO_D1);
	gpio_export(GPIO_D2);
	gpio_export(GPIO_D3);
	gpio_export(GPIO_D4);
	gpio_export(GPIO_D5);
	gpio_export(GPIO_D6);
	gpio_export(GPIO_D7);
	gpio_export(GPIO_CS);
	gpio_export(GPIO_DC);
	gpio_export(GPIO_WR);
	gpio_export(GPIO_RD);
	gpio_export(GPIO_RS);

	gpio_set_dir(GPIO_D0,OUTPUT_PIN);
	gpio_set_dir(GPIO_D1,OUTPUT_PIN);
	gpio_set_dir(GPIO_D2,OUTPUT_PIN);
	gpio_set_dir(GPIO_D3,OUTPUT_PIN);
	gpio_set_dir(GPIO_D4,OUTPUT_PIN);
	gpio_set_dir(GPIO_D5,OUTPUT_PIN);
	gpio_set_dir(GPIO_D6,OUTPUT_PIN);
	gpio_set_dir(GPIO_D7,OUTPUT_PIN);
	gpio_set_dir(GPIO_CS,OUTPUT_PIN);
	gpio_set_dir(GPIO_DC,OUTPUT_PIN);
	gpio_set_dir(GPIO_WR,OUTPUT_PIN);
	gpio_set_dir(GPIO_RD,OUTPUT_PIN);
	gpio_set_dir(GPIO_RS,OUTPUT_PIN);

	gpio_set_value(GPIO_D0,LOW);
	gpio_set_value(GPIO_D1,LOW);
	gpio_set_value(GPIO_D2,LOW);
	gpio_set_value(GPIO_D3,LOW);
	gpio_set_value(GPIO_D4,LOW);
	gpio_set_value(GPIO_D5,LOW);
	gpio_set_value(GPIO_D6,LOW);
	gpio_set_value(GPIO_D7,LOW);
	gpio_set_value(GPIO_CS,HIGH);
	gpio_set_value(GPIO_DC,HIGH);
	gpio_set_value(GPIO_WR,HIGH);
	gpio_set_value(GPIO_RD,HIGH);
	gpio_set_value(GPIO_RS,HIGH);
}

void ReleaseGpio()
{
	gpio_unexport(GPIO_D0);
	gpio_unexport(GPIO_D1);
	gpio_unexport(GPIO_D2);
	gpio_unexport(GPIO_D3);
	gpio_unexport(GPIO_D4);
	gpio_unexport(GPIO_D5);
	gpio_unexport(GPIO_D6);
	gpio_unexport(GPIO_D7);
	gpio_unexport(GPIO_CS);
	gpio_unexport(GPIO_DC);
	gpio_unexport(GPIO_WR);
	gpio_unexport(GPIO_RD);
	gpio_unexport(GPIO_RS);
}
