#include <LowPower.h>

#include <Wire.h>
#include <DS3231.h>

#define LED 13
DS3231 clock;
RTCDateTime dt;

long nextTick = 0;

void setup(){
  for (int i=0; i<20; i++){
    if (i == 1) continue;
    pinMode(i, OUTPUT);
  }
  Serial.begin(9600);
   clock.begin();
  clock.setDateTime(0);
 

  
}

void printDouble( double val, byte precision){
 // prints val with number of decimal places determine by precision
 // precision is a number from 0 to 6 indicating the desired decimial places
 // example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)

 Serial.print (int(val));  //prints the int part
 if( precision > 0) {
   Serial.print("."); // print the decimal point
   unsigned long frac;
   unsigned long mult = 1;
   byte padding = precision -1;
   while(precision--)
      mult *=10;
     
   if(val >= 0)
     frac = (val - int(val)) * mult;
   else
     frac = (int(val)- val ) * mult;
   unsigned long frac1 = frac;
   while( frac1 /= 10 )
     padding--;
   while(  padding--)
     Serial.print("0");
   Serial.print(frac,DEC) ;
 }
}

void pretty_time(long int duration, char* buff) {
	/* 
 	 * Convert Arduino's time in milliseconds
 	 * to human readable values
 	 */

	long millis = duration % 1000;
	long second = (duration / 1000) % 60;
	long minute = (duration / (1000 * 60UL)) % 60;
	long hour = (duration / (1000 * 60UL * 60UL)) % 24;
	char nice[13];	
	sprintf(nice, "%02ld:%02ld:%02ld.%03ld", hour, minute, second, millis);
	strncpy(buff, nice, sizeof(nice));
	
}


double ratio = 1.058594;
unsigned long timeDelta = 0;

/* Inaccurate way of entering deep sleep for X ms 
 * while keep the time kindof accurate 
 */
void sleep(long ms, double ratio){

     for (int i=ms/500; i>0; i--) {
       LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
       timeDelta += 500 * ratio; // Ratio minimize the gap between real-time 
                                // and shitty calcultations
                                // let's call it good enough
    }

}

/* Inaccurate way of keeping track of millis()
 * after some deep sleep
 */
long now() {
   return (millis() + timeDelta );
}

void loop(){

  digitalWrite(LED, !digitalRead(LED));
  sleep(1000, ratio);
  dt = clock.getDateTime();
  
  if ( dt.unixtime != now()/1000 ){
    Serial.print("RTC time:");
    Serial.println(dt.unixtime);
    Serial.print("Calculated Time:");
    Serial.println(now()/1000);
    Serial.println("====================");
    Serial.flush();
  }
  
}
