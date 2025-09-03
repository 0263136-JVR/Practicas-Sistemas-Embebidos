#include <pigpio.h>
#include <stdio.h>

#define BUTTON 16

int main(void){
	if(gpioInitialise() < 0){
		printf("Failed to Initialize pigpio\n");
		return -1;
	}
	
	gpioSetMode(BUTTON,PI_INPUT);
	gpioSetPullUpDown(BUTTON,PI_PUD_DOWN);
	
	int hSerial = serOpen("/dev/ttyS0", 115200, 0);
	char Send[15] = "Empanadasaurio\n";
	char receive[100];
	int go = 1;
	//int flag = 0;

	while(go){
		if(gpioRead(BUTTON)){
			serWrite(hSerial, Send, sizeof(Send));
			//flag = 1;
			gpioDelay(100000);
		}
		int Data = serDataAvailable(hSerial);

		if(Data){
			serRead(hSerial, receive, Data);
			printf("%s", receive);
			serClose(hSerial);
			printf("Success!!!!!!\n");
			go = 0;
		}
	}
	gpioTerminate();

	return 0;
}

