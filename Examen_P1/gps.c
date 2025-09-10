#include <pigpio.h>
#include <stdio.h>
#include <math.h>
#include "minmea.h"

#define BUTTON 16

const int R = 6371000;
const float pi = 3.1416;

struct minmea_sentence_rmc coords;
struct minmea_sentence_gga alti;
float CenterLat =/* 20.6802778;*/20.682356;
float CenterLong =/* -103.4431389;*/-103.417757;


int main(void){
	float CenterLatr = (CenterLat * pi) / 180;
	if(gpioInitialise() < 0){
		printf("Failed to Initialize pigpio\n");
		return -1;
	}
	
	gpioSetMode(BUTTON,PI_INPUT);
	gpioSetPullUpDown(BUTTON,PI_PUD_DOWN);
	gpioPWM(17,0);
	gpioSetPWMfrequency(17,1000);
	
	int hSerial = serOpen("/dev/ttyS0",/* 115200*/9600, 0);
	char receiver[100];
	int go = 1;
	int state = 1;
	int pos = 0;
	int time = 0;
	int time2 = 0;
	int timeg = 0;
	int tick = 0;
	int on = 0;
	int hora = 23;
	int min = 59;
	int seg = 59;
	float lat = CenterLat;
	float lon = CenterLong;
	float vel = 0;
	float alt = 0;
	float dis = 999;

	while(go){
		timeg = gpioTick();
		int Data = serDataAvailable(hSerial);
		//serReadByte();
		if(Data){
			char temp = serReadByte(hSerial);
			switch(state){
				case 1:
					if(temp == '$'){
						receiver[pos] = temp;
						state = 2;
						pos++;
					}
					break;
				case 2:
					if(temp != '\r'){
						receiver[pos] = temp;
						pos++;
					}else{
						receiver[pos] = temp;
						state = 3;
						pos++;
					}
					break;
				case 3:
					if(temp == '\n'){
						receiver[pos] = temp;
						receiver[pos + 1] = '\0';
						if(receiver[3] == 'R'){
							minmea_parse_rmc(&coords,receiver);
							hora = coords.time.hours;
							min = coords.time.minutes;
							seg = coords.time.seconds;
							lat = minmea_tocoord(&coords.latitude);
							lon = minmea_tocoord(&coords.longitude);
							vel = minmea_tofloat(&coords.speed);
							// printf("%.7f,%.7f,%.7f\n",lat,lon, vel);
						}
						if(receiver[4] == 'G'){
							minmea_parse_gga(&alti, receiver);
							alt = minmea_tocoord(&alti.altitude);
							// printf("%.7f\n",alt);
						}
						state = 1;
						pos = 0;
						//

						vel = vel *0.51444;
						float latr = (lat * pi) / 180;
						float deltaLat = lat - CenterLat;
						float deltaLatr = (deltaLat * pi) / 180;
						float deltaLon = lon - CenterLong;
						float deltaLonr = (deltaLon * pi) / 180;
						float num = 1 - cos(deltaLatr) + cos(CenterLatr) * cos(latr) * (1 - cos(deltaLonr));
						num = num / 2;
						float raiz = sqrt(num);
						dis = 2 * R * asin(raiz);

					}else{
						state = 1;
						pos = 0;
					}
					break;
			}
			
		}
		if(dis < 10){
			tick = 100000 / 2;
		}else if(dis >= 10 && dis < 30){
			tick = 500000 / 2;
		}else if(dis >= 30 && dis <= 50){
			tick = 1000000 / 2;
		}else{
			tick = 0;
		}
		if((timeg - time) > tick){
			if(on || dis > 50){
				gpioPWM(17,0);
				on = 0;
				time = gpioTick();
			}else if(!on){
				gpioPWM(17,128);
				on = 1;
				time = gpioTick();
			}
		}
		if((timeg - time2 > 500000)){
			printf("%d:%d:%d, %.6f, %.6f, %.6f, %.6f, %.6f\n", hora, min, seg, lat, lon, alt, vel, dis);
			time2 = gpioTick();
		}
	}
	serClose(hSerial);
	gpioTerminate();

	return 0;
}

