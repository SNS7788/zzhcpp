
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
 
typedef unsigned char uint8;
typedef unsigned int  uint16;
typedef unsigned long uint32;
 
#define HIGH_TIME 32

#define DATAPIN 0
uint32 databuf;

 //duohexin

 
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
 
  //printf("Use GPIO1 to read data!\n");
 
  if (-1 == wiringPiSetup()) {
    printf("Setup wiringPi failed!");
    return 1;
  }
 
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



