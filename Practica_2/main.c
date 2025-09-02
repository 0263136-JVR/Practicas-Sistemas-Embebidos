#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>

//Define gpios for activation of display
#define DISPLAY_1 17
#define DISPLAY_2 27
#define DISPLAY_3 22

//Define gpios for 7 segments control
#define SEG_A 5
#define SEG_B 6
#define SEG_C 26
#define SEG_D 23
#define SEG_E 24
#define SEG_F 25
#define SEG_G 16

float core_read_temperature(void){
	FILE *fp;
	char buf[128];
	float temp = 0.0F;

	//Open vcgencmd to read temp
	fp = popen("vcgencmd measure_temp", "r");
	if(fp == NULL){
		return -1.0F;
	}

	//Get Result
	if(fgets(buf, sizeof(buf), fp) != NULL){
		//Convert float
		sscanf(buf, "temp=%f", &temp);
		printf("Temp CPU: %.2f C\n", temp);
	}

	pclose(fp);

	return temp;
}

int main(void){
	float temp, tempx10;
	int tempInt, dec, uni, deci, signal;
	int cont = 0;
	int timer = 0;
	if(gpioInitialise() < 0){
		printf("Failed to Initialize");
		return -1;
	}

	while(1){
		switch(cont){
			case 0:
				temp = core_read_temperature();
				tempInt = temp;
				dec = tempInt / 10;
				uni = tempInt - (dec * 10);
				tempx10 = temp * 10;
				deci = tempx10 - (dec * 100) - (uni * 10);
				timer = 0;
			case 1:
				signal = dec;
				gpioWrite(DISPLAY_3, 1);
				gpioWrite(DISPLAY_2, 0);
				gpioWrite(DISPLAY_1, 0);
				cont = 2;
				break;
			case 2:
				signal = uni;
				gpioWrite(DISPLAY_3, 0);
				gpioWrite(DISPLAY_2, 1);
				gpioWrite(DISPLAY_1, 0);
				cont = 3;
				break;
			case 3:
				signal = deci;
				gpioWrite(DISPLAY_3, 0);
				gpioWrite(DISPLAY_2, 0);
				gpioWrite(DISPLAY_1, 1);
				if(timer >= 100){
					cont = 0;
				}else{
					cont = 1;
				}
				break;
		}
		switch(signal){
			case 0:
				gpioWrite(SEG_A, 1);
				gpioWrite(SEG_B, 1);
				gpioWrite(SEG_C, 1);
				gpioWrite(SEG_D, 1);
				gpioWrite(SEG_E, 1);
				gpioWrite(SEG_F, 1);
				gpioWrite(SEG_G, 0);
				break;
			case 1:
				gpioWrite(SEG_A, 0);
				gpioWrite(SEG_B, 1);
				gpioWrite(SEG_C, 1);
				gpioWrite(SEG_D, 0);
				gpioWrite(SEG_E, 0);
				gpioWrite(SEG_F, 0);
				gpioWrite(SEG_G, 0);
				break;
			case 2:
				gpioWrite(SEG_A, 1);
				gpioWrite(SEG_B, 1);
				gpioWrite(SEG_C, 0);
				gpioWrite(SEG_D, 1);
				gpioWrite(SEG_E, 1);
				gpioWrite(SEG_F, 0);
				gpioWrite(SEG_G, 1);
				break;
			case 3:
				gpioWrite(SEG_A, 1);
				gpioWrite(SEG_B, 1);
				gpioWrite(SEG_C, 1);
				gpioWrite(SEG_D, 1);
				gpioWrite(SEG_E, 0);
				gpioWrite(SEG_F, 0);
				gpioWrite(SEG_G, 1);
				break;
			case 4:
				gpioWrite(SEG_A, 0);
				gpioWrite(SEG_B, 1);
				gpioWrite(SEG_C, 1);
				gpioWrite(SEG_D, 0);
				gpioWrite(SEG_E, 0);
				gpioWrite(SEG_F, 1);
				gpioWrite(SEG_G, 1);
				break;
			case 5:
				gpioWrite(SEG_A, 1);
				gpioWrite(SEG_B, 0);
				gpioWrite(SEG_C, 1);
				gpioWrite(SEG_D, 1);
				gpioWrite(SEG_E, 0);
				gpioWrite(SEG_F, 1);
				gpioWrite(SEG_G, 1);
				break;
			case 6:
				gpioWrite(SEG_A, 1);
				gpioWrite(SEG_B, 0);
				gpioWrite(SEG_C, 1);
				gpioWrite(SEG_D, 1);
				gpioWrite(SEG_E, 1);
				gpioWrite(SEG_F, 1);
				gpioWrite(SEG_G, 1);
				break;
			case 7:
				gpioWrite(SEG_A, 1);
				gpioWrite(SEG_B, 1);
				gpioWrite(SEG_C, 1);
				gpioWrite(SEG_D, 0);
				gpioWrite(SEG_E, 0);
				gpioWrite(SEG_F, 0);
				gpioWrite(SEG_G, 0);
				break;
			case 8:
				gpioWrite(SEG_A, 1);
				gpioWrite(SEG_B, 1);
				gpioWrite(SEG_C, 1);
				gpioWrite(SEG_D, 1);
				gpioWrite(SEG_E, 1);
				gpioWrite(SEG_F, 1);
				gpioWrite(SEG_G, 1);
				break;
			case 9:
				gpioWrite(SEG_A, 1);
				gpioWrite(SEG_B, 1);
				gpioWrite(SEG_C, 1);
				gpioWrite(SEG_D, 1);
				gpioWrite(SEG_E, 0);
				gpioWrite(SEG_F, 1);
				gpioWrite(SEG_G, 1);
				break;
		}
		timer++;
		gpioDelay(10000);
	}




	gpioTerminate();

	return 0;
}
