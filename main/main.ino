//We always have to include the library
#include "LedControl.h"
#include "Wire.h"
#include "DS3231.h"

RTClib myRTC;

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 7 is connected to the DataIn 
 pin 6 is connected to the CLK 
 pin 5 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(7,6,5,1);

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;

void setup() {
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
  Serial.begin(9600);
  Wire.begin();
}


/*
  This method will scroll all the hexa-decimal
 numbers and letters on the display. You will need at least
 four 7-Segment digits. otherwise it won't really look that good.
 */
void scrollDigits() {
  for(int i=0;i<13;i++) {
    lc.setDigit(0,3,i,false);
    lc.setDigit(0,2,i+1,false);
    lc.setDigit(0,1,i+2,false);
    lc.setDigit(0,0,i+3,false);
    delay(delaytime);
  }
  lc.clearDisplay(0);
  delay(delaytime);
}

/*
 * display the 8 least significant whole digits of a given number
 */
void display_double(double number) {
  long head = (((long) number) % 100000000);
  Serial.println(number);
  for (int i = 0; i < 8; i++) {
    lc.setDigit(0,i, head%(10),false);
    Serial.println(head%(10));
    head = head / 10;
  }
}

void loop () {
  
    delay(1000);
  
    DateTime now = myRTC.now();
    double unixtime = now.unixtime();
    display_double(unixtime);
    
    Serial.println(now.unixtime());
}
