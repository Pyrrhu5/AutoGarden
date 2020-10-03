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


//double ratio = 1.058699;
double ratio = 1.065;
double upperLimit = 1.07;
double lowerLimit = 1.058593;
unsigned int nIter = 0;
double deltaMili = 0;

/* Inaccurate way of entering deep sleep for X ms 
 * while keep the time kindof accurate 
 */
void sleep(long ms, double ratio){

     for (int i=ms/500; i>0; i--) {
       LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
       nIter += 1; // Ratio minimize the gap between real-time 
                                // and shitty calcultations
                                // let's call it good enough
    }

}

/* Inaccurate way of keeping track of millis()
 * after some deep sleep
 */
long now() {
   return (millis() - deltaMili + (500.0 * nIter * ratio));
}



double bruteforce_ratio(double currentRatio, long rtcVal, long timeMs) {

  unsigned int calcVal = (timeMs + (500 * nIter * ratio)) / 1000;
  
  while (rtcVal != calcVal){
    
    // ratio needs to be increased
    if (rtcVal > calcVal ) {
      lowerLimit = currentRatio;
      currentRatio = ((upperLimit + currentRatio) / 2);
      
    } 
    // ratio needs to be decreased
    else if (rtcVal < calcVal ) {
      upperLimit = currentRatio;
      currentRatio = ( (lowerLimit + currentRatio ) / 2);
    }
    // ratio is not being updated anymore
    if (ratio == currentRatio) break;
    calcVal = (timeMs + (500 * nIter * currentRatio)) / 1000;
    
  }

  return currentRatio;
}



void loop(){

   
  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_ON, 
  //              SPI_OFF, USART0_OFF, TWI_OFF);
  digitalWrite(LED, !digitalRead(LED));
  sleep(1000, ratio);

  dt = clock.getDateTime();
  if ( dt.unixtime != now()/1000 ){
    Serial.print("RTC time:");
    Serial.println(dt.unixtime);
    Serial.print("Calculated Time:");
    Serial.println(now()/1000);
    Serial.print("Old ratio: ");
    printDouble(ratio, 6);
    Serial.println();
    Serial.flush();
    ratio = bruteforce_ratio(ratio, dt.unixtime, millis() - deltaMili);
    Serial.print("New ratio: ");
    printDouble(ratio, 6);
    Serial.println();
    Serial.println("====================");
    Serial.flush();
  }
  
}
