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

int VOLTMETER_PIN = 28;

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

  pinMode(VOLTMETER_PIN, OUTPUT);
}

int current_voltmeter = 0;
void display_on_voltmeter(float current, float minimum, float maximum) {
  /* display a value between min and max on the voltmeter*/

  float multiplier = (current - minimum) / (maximum - minimum);

  float write_value = 255 * multiplier;
  
  // interpolate between the previous and new value
  for (float i = 0; i <= 4; i++) {
    int interpolated_value = current_voltmeter + (i/4)*(write_value - current_voltmeter);
    delay(100);
    analogWrite(VOLTMETER_PIN, (int) interpolated_value);
    Serial.println(interpolated_value);
  }

  current_voltmeter = write_value;
  
  
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
  
    delay(500);
    DateTime now = myRTC.now();
    double unixtime = now.unixtime();
    display_double(unixtime);

    display_on_voltmeter(now.second(), 0, 60);
    Serial.println(now.unixtime());
}
