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


/* 
  ---------------------------------------------------------------------
  Moon calculations - taken from  Astronomical algorithms by Jean Meeus 
  ---------------------------------------------------------------------
*/

/* constants */
int D_coefficients [] = {0, 2, 2, 0, 0, 0, 2, 2, 2, 2, 0, 1, 0, 2, 0, 0, 4, 0, 4, 2, 2, 1, 1, 2, 2, 4, 2, 0, 2, 2, 1, 2, 0, 0, 2, 2, 2, 4, 0, 3, 2, 4, 0, 2, 2, 2, 4, 0, 4, 1, 2, 0, 1, 3, 4, 2, 0, 1, 2, 2};
int M_coefficients [] = {0, 0, 0, 0, 1, 0, 0, -1, 0, -1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, -1, 0, 0, 0, 1, 0, -1, 0, -2, 1, 2, -2, 0, 0, -1, 0, 0, 1, -1, 2, 2, 1, -1, 0, 0, -1, 0, 1, 0, 1, 0, 0, -1, 2, 1, 0, 0};
int M_prime_coefficients [] = {1, -1, 0, 2, 0, 0, -2, -1, 1, 0, -1, 0, 1, 0, 1, 1, -1, 3, -2, 1, 0, -1, 0, 1, 2, 0, -3, -2, -1, -2, 1, 0, 2, 0, -1, 1, 0, -1, 2, -1, 1, -2, -1, -1, -2, 0, 1, 4, 0, -2, 0, 2, 1, -2, -3, 2, 1, -1, 3, -1};
int F_coefficients [] = {0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, -2, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, -2, 2, 0, 2, 0, 0, 0, 0, 0, 0, -2, 0, 0, 0, 0, -2, -2, 0, 0, 0, 0, 0, 0, 0, -2};
int cos_coefficients [] = {-20905355, -3699111, -2955968, -569925, 48888, -3149, 246158, -152138, -170733, -204586, -129620, 108743, 104755, 10321, 0, 79661, -34782, -23210, -21636, 24208, 30824, -8379, -16675, -12831, -10445, -11650, 14403, -7003, 0, 10056, 6322, -9884, 5751, 0, -4950, 4130, 0, -3958, 0, 3258, 2616, -1897, -2117, 2354, 0, 0, -1423, -1117, -1571, -1739, 0, -4421, 0, 0, 0, 0, 1165, 0, 0, 8752};
int coefficient_length = 60;
double reduce_angle(double angle) {
  /* reduces the angle to be in [0, 360)*/

  return angle - 360 * floor(angle / 360);
}


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

double find_E(double T) {
  /* calculates the eccentricity of earth's orbit around the sun correction */
  double E = 1 - 0.002516*T - 0.0000074*pow(T,2);
  return E;
}

double cosine_in_degrees(double angle_in_degrees) {
  /* returns the cosine of an angle given in degrees */

  return cos((angle_in_degrees) * (M_PI / 180.0));
}

long calculate_moon_distance(double seconds_since_epoch) {
  /* returns the distance to the moon in at the given seconds since epoch in km*/
  double T = find_T_since_julian_epoch(seconds_since_epoch);
  Serial.println("---calculations---");
  Serial.println(T,10);
  Serial.println(find_D(T),10);
  Serial.println(find_M(T),10);
  Serial.println(find_M_prime(T),10);
  Serial.println(find_F(T),10);
  Serial.println(find_E(T), 10);
  Serial.println("------");

  double D = find_D(T);
  double M = find_M(T);
  double M_prime = find_M_prime(T);
  double F = find_F(T);
  double E = find_E(T);

  double sum_r = 0;

  for (int i = 0; i < coefficient_length; i++) {
    double new_term = cos_coefficients[i]*cosine_in_degrees(
                                        D_coefficients[i]*D + 
                                        M_coefficients[i]*M + 
                                        M_prime_coefficients[i]*M_prime +
                                        F_coefficients[i]*F);

    //perform the eccentricity correction
    if (M_coefficients[i] == 1 || M_coefficients[i] == -1) { new_term *= E; }
    else if (M_coefficients[i] == 2 || M_coefficients[i] ==  -2) { new_term *= pow(E,2); }

    sum_r += new_term;
  }

  double distance = 385000.56 + sum_r/1000;
  Serial.println(distance);
  return (long) distance;
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
void display_on_voltmeter(double current, double minimum, double maximum) {
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
void display_long(long number) {
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
    
    Serial.println(now.unixtime());

    long moon_distance = calculate_moon_distance((double) now.unixtime());
    display_long(moon_distance);
    display_on_voltmeter((double) moon_distance, 356400, 406700);

    
}
