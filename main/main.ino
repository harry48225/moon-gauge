//We always have to include the library
#include "LedControl.h"
#include "Wire.h"
#include "DS3231.h"
#include "math.h"

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

double reduce_angle(double angle) {
  /* reduces the angle to be in [0, 360)*/

  return angle - 360 * floor(angle / 360);
}

/* Moon calculations - taken from  Astronomical algorithms by Jean Meeus */
double find_T_since_julian_epoch(double seconds_since_epoch) {
  return (seconds_since_epoch - 946727936)/((double) 60*60*24*36525);
}

double find_D(double T) {
  /* calculates the mean elongation of the Moon */
  double D = 297.8501921 + 445267.1114034*T - 0.0018819*pow(T,2) + pow(T,3)/545868 - pow(T,4)/113065000;
  return reduce_angle(D);
}

double find_M(double T) {
  /* calculates the Sun's mean anomaly */
  double M = 357.5291092 + 35999.0502909*T - 0.0001536*pow(T,2) + pow(T,3)/24490000;
  return reduce_angle(M);
}

double find_M_prime(double T) {
  /* calculates the Moon's mean anomaly */
  double M_prime = 134.9633964 + 477198.8675055*T + 0.0087414*pow(T,2) + pow(T,3)/69699 - pow(T,4)/14712000;
  return reduce_angle(M_prime);
}

double find_F(double T) {
  /* calculates the moon's argument of latitude (mean distance of the moon from its ascending node) */
  double F = 93.2720950 + 483202.0175233*T - 0.0036539*pow(T,2) - pow(T,3)/3526000 + pow(T,4)/863310000;
  return reduce_angle(F);
}


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

    double T = find_T_since_julian_epoch((double) now.unixtime());
    Serial.println("---calculations---");
    Serial.println(T,10);
    Serial.println(find_D(T),10);
    Serial.println(find_M(T),10);
    Serial.println(find_M_prime(T),10);
    Serial.println(find_F(T),10);
    Serial.println("------");
}
