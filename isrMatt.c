#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
 int backLight;
int textDisplay;

typedef unsigned char uint8;
typedef unsigned int  uint16;
typedef unsigned long uint32;
 
#define HIGH_TIME 32

#define DATAPIN 0
uint32 databuf;

int r=15,g=100,b=255;

void setBGColor(unsigned char red, unsigned char green, unsigned blue)
{
	// Initialize.
	wiringPiI2CWriteReg8(backLight, 0x00, 0x00);
	wiringPiI2CWriteReg8(backLight, 0x01, 0x00);

	// All LED control by PWM.
	wiringPiI2CWriteReg8(backLight, 0x08, 0xAA);

	// Set color component.
	wiringPiI2CWriteReg8(backLight, 0x04, red);
	wiringPiI2CWriteReg8(backLight, 0x03, green);
	wiringPiI2CWriteReg8(backLight, 0x02, blue);
}
// Internal use only.
void textCommand(unsigned char cmd)
{
    wiringPiI2CWriteReg8(textDisplay, 0x80, cmd);
}

// Set text of diaplay.
int setText(const char *string)
{
	// Clear display.
	textCommand(0x01);

	delay(50);

	// Display on, no cursor.
    textCommand(0x08 | 0x04);

    // Display two lines.
    textCommand(0x28);

    delay(50);

    int count = 0;
    int row = 0;
    int i = 0;

    for (; string[i] != '\0'; ++i) {
        if ((string[i] == '\n') || (count == 16)) {
            count = 0;
            ++row;
            if (row == 2) {
				// Reach maximum line number. Truncate any characters behind.
                break;
			}
            textCommand(0xc0);
            if (string[i] == '\n') {
                continue;
			}
		}
        ++count;
        wiringPiI2CWriteReg8(textDisplay, 0x40, string[i]);
	}
}

void sigIntHandler(int signal)
{
	// Clear screen.
	textCommand(0x01);

	// Disable backlight.
	setBGColor(0, 0, 0);

	printf("\nProgram ended with exit value -2.\n");
	exit(-2);
}
 
void readSensorData(void)
{
    uint8 crc; 
    uint8 i;
 
   /* pinMode(DATAPIN ,OUTPUT);
    digitalWrite(DATAPIN, 0); //low  for 25ms 
    delay(25);
    digitalWrite(DATAPIN, 1);*/ //set high 
    pinMode(DATAPIN, INPUT); // input
    pullUpDnControl(DATAPIN, PUD_UP);// wait for sensor response
 
    delayMicroseconds(27);//delay 27us
    if(digitalRead(DATAPIN)==0) //SENSOR ANS
       {
         while(!digitalRead(DATAPIN)); //wait to high
 
      for(i=0;i<32;i++)
       {
       while(digitalRead(DATAPIN)); //data clock start
       while(!digitalRead(DATAPIN)); //data start
          delayMicroseconds(HIGH_TIME);
          databuf*=2;
           if(digitalRead(DATAPIN)==1) //1
            {
                databuf++;
            }
        }
 
      for(i=0;i<8;i++)
       {
       while(digitalRead(DATAPIN)); //data clock start
       while(!digitalRead(DATAPIN)); //data start
          delayMicroseconds(HIGH_TIME);
          crc*=2;  
          if(digitalRead(DATAPIN)==1) //1
           {
                crc++;
           }
        }
	
	int checksum_gen = ((databuf>>24)&0xff) + ((databuf>>16)&0xff) +((databuf>>8)&0xff) +(databuf&0xff)+1;
	int checksum = crc&0xff;
	
	if(checksum_gen == checksum){


       printf("Room Humidity:   %d.%d\n",((databuf>>24)&0xff)/2,((databuf>>16)&0xff)/2);
      printf("Room Tempurature:%d.%d\n\n",((databuf>>8)&0xff)/2,(databuf&0xff)/2);
       char *reading;
       asprintf(&reading, "H: %d.%d\n T: %d.%d \n", ((databuf>>24)&0xff)/2, ((databuf>>16)&0xff)/2, ((databuf>>8)&0xff)/2, ((databuf)&0xff)/2);
         setText(reading);


    	setBGColor(r, g, b);
        r+=10;
        g+=10;b-=10;
        if(r==255) r=10;
        if(g==250) g=100;if(b==15) b=255;
       databuf=0;
	crc = 0;
}
	else {
	    printf("did not complete checksum\n");
		databuf = 0;
		crc = 0;
       }
}
   else
        {
	
         }
}
 
int main (void)
{
 // Catch signal (Ctrl + C).
	struct sigaction sigHandler;
	sigHandler.sa_handler = sigIntHandler;
	sigemptyset(&sigHandler.sa_mask);
    sigHandler.sa_flags = 0;
    sigaction(SIGINT, &sigHandler, NULL);
  //printf("Use GPIO1 to read data!\n");
 
  if (-1 == wiringPiSetup()) {
    printf("Setup wiringPi failed!");
    return 1;
  }
  	backLight = wiringPiI2CSetup(0x62);
	textDisplay = wiringPiI2CSetup(0x3E);

    	if((backLight == -1) || (textDisplay == -1)) {
		fprintf(stderr, "Failed to initialize I2C device. Exit.\n");
		printf("Program ended with exit value -1.\n");
		return -1;
        }

   setText("Hello:\n      World!");

	int colorCount = 0;

	unsigned char r = (unsigned char)255;
	unsigned char g = (unsigned char)0;
	unsigned char b = (unsigned char)0;
    	setBGColor(r, g, b);
        delay(10);
 // pinMode(DATAPIN, OUTPUT); 
 // digitalWrite(DATAPIN, 1); // high  
 
 // printf("Enter OS-------\n");
 // while(1) {
  // pinMode(DATAPIN, OUTPUT); // set mode to output
   // digitalWrite(DATAPIN, 1); // output a high level 
   // delay(2000);
	//printf("we are trying our best\n");
    wiringPiISR(DATAPIN, INT_EDGE_RISING, readSensorData);

while(1){
//printf("did it make it here?\n");
// try to start a communication
 /* pinMode(DATAPIN ,OUTPUT);
    digitalWrite(DATAPIN, 0); //low for 25ms 
    delay(25);
    digitalWrite(DATAPIN, 1); //set high
    pinMode(DATAPIN, INPUT); // input
    pullUpDnControl(DATAPIN, PUD_UP);
    delay(3000);
pinMode(DATAPIN, OUTPUT);
digitalWrite(DATAPIN, 0);
delay(3000);
digitalWrite(DATAPIN, 1);
delay(3000);*/

    pinMode(DATAPIN ,OUTPUT);
    digitalWrite(DATAPIN, 0); //low  for 25ms 
    delay(25);
    digitalWrite(DATAPIN, 1);

	

}

  return 0;
}



