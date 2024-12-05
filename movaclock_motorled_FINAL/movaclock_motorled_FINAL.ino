//20241201 MOVACLOCK - Ka Man CHOI JANET

//Chinese Word Clock https://www.instructables.com/Chinese-Word-Clock
//Arduino + Neopixel Word Clock - Super Make Something Episode 6 https://www.youtube.com/watch?v=6tMTQ7Khe90

/***************************************************
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 8 servos, one after the other on the
  first 8 pins of the PCA9685

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These drivers use I2C to communicate, 2 pins are required to
  interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Time.h>
#include <TimeLib.h>


#include <SoftwareSerial.h>
SoftwareSerial SwSerial(10, 11);  // RX, TX

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN 3
#define NUMPIXELS 35
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define MINUSFIVEMINS 4
#define PLUSFIVEMINS 5
#define BUTTONLEDMINUS_GND1 8
#define BUTTONLEDMINUS_GND2 9
#define BUTTONLEDPLUS_GND1 10
#define BUTTONLEDPLUS_GND2 11
#define BUTTONLEDMINUS_5V1 A0
#define BUTTONLEDMINUS_5V2 A1
#define BUTTONLEDPLUS_5V1 A2
#define BUTTONLEDPLUS_5V2 A3
int minusPrevState = HIGH;
int minusCurrState = HIGH;
int plusPrevState = HIGH;
int plusCurrState = HIGH;
// Time variables
int h;
int m;
int s;
// RGB color variables
int red = 255;
int green = 255;
int blue = 255;



#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// 3 pcs of PCA9685
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(0x42);

#define TOTAL_SERVOS 35
// Array to keep track of current servo states
// 0 = DOWN, 1 = UP
bool servoStates[TOTAL_SERVOS] = {0};

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096) HIGHEST TEXT
#define SERVOMAX  500 // This is the 'maximum' pulse length count (out of 4096) LOWEST TEXT

#define UP  200 // This is the 'minimum' pulse length count (out of 4096) HIGHEST TEXT
#define DOWN  500 // This is the 'maximum' pulse length count (out of 4096) LOWEST TEXT
#define USMIN  1200 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define puttype  600 // highest for putting the type
#define upclock  1200 // highest for clock
#define USMAX  2000 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define downclock  2000 // lowest for clock
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// our servo # counter
uint8_t servonum = 0;
uint8_t servonumMAX = 16;

void setup() {
  Serial.begin(9600);
  Serial.println("8 channel Servo test!");

  pwm.begin();
  pwm2.begin();
  pwm3.begin();
  pixels.begin();

  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  pwm2.setOscillatorFrequency(27000000);
  pwm2.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  pwm3.setOscillatorFrequency(27000000);
  pwm3.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  //set time!
  setTime(3, 16, 0, 30, 11, 2024);  //Initialize current time as Midnight/noon 08/31/2015

  pixels.begin();                   //Begin Neopixel string

  delay(10);

  //PIXEL MATRIX
  pixels.clear();
  delay(2000);
  for (int i = NUMPIXELS; i >= 0; i--) { // For each pixel...
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(10); // Pause before next pass through loop
  }

  // Reset all servos to UP position
  for (int i = 0; i < TOTAL_SERVOS; i++) {
    pwm.setPWM(i, 0, UP);
    pwm2.setPWM(i, 0, UP);
    pwm3.setPWM(i, 0, UP);
    delay(50);
  }

  delay(800);  // Small delay between each servo to prevent power surge

  // Reset all servos to DOWN position
  for (int i = 0; i < TOTAL_SERVOS; i++) {
    pwm.setPWM(i, 0, DOWN);
    pwm2.setPWM(i, 0, DOWN);
    pwm3.setPWM(i, 0, DOWN);
  }

  delay(500);  // Small delay between each servo to prevent power surge

  //PIXEL MATRIX
  pixels.clear();
  delay(2000);
  for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(100); // Pause before next pass through loop
  }

  delay(2000);
}

void loop() {

  // Drive each servo one at a time using setPWM()
  //DOWN TEXT
  Serial.println(servonum);

  //downtext();
  //uptext();

  movaclock();
}

//function for making the types down
void downtext() {
  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
    pwm.setPWM(servonum, 0, pulselen);
    pwm2.setPWM(servonum, 0, pulselen);
    pwm3.setPWM(servonum, 0, pulselen);
  }

  delay(500);
}

void uptext() {
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
    pwm.setPWM(servonum, 0, pulselen);
    pwm2.setPWM(servonum, 0, pulselen);
    pwm3.setPWM(servonum, 0, pulselen);
  }

  delay(500);
}

void allup() {
  pwm.setPWM(servonum, 0, UP);
  pwm2.setPWM(servonum, 0, UP);
  pwm3.setPWM(servonum, 0, UP);

  servonum++;
  if (servonum > servonumMAX) servonum = 0; // Testing the first 8 servo channels

}

void alldown() {
  pwm.setPWM(servonum, 0, DOWN);
  pwm2.setPWM(servonum, 0, DOWN);
  pwm3.setPWM(servonum, 0, DOWN);

  servonum++;
  if (servonum > servonumMAX) servonum = 0; // Testing the first 8 servo channels

}

void movaclock() {
  //Declare integer array with size corresponding to number of Neopixels in chain
  int individualPixels[NUMPIXELS] = { 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0
                                    };

  /* Check for button presses & reset time if necessary */
  minusCurrState = digitalRead(MINUSFIVEMINS);  //Get current state of MINUSFIVEMINS button
  /* If current state is different from previous state and value is now LOW, subtract five minutes from current time */
  if ((minusCurrState != minusPrevState) && (minusCurrState == LOW)) {
    //adjustTime(-5*60); //Shift time five minutes backwards
    adjustTime(-1 * 60);  //Shift time one minutes backwards

    minusPrevState = minusCurrState;
  } else {
    minusPrevState = minusCurrState;
  }

  /* If current state is different from previous state and value is now LOW, add five minutes from current time */
  if ((plusCurrState != plusPrevState) && (plusCurrState == LOW)) {
    //adjustTime(5 * 60); //Shift time five minutes forwards
    adjustTime(1 * 60);  //Shift time one minutes forwards

    plusPrevState = plusCurrState;
  } else {
    plusPrevState = plusCurrState;
  }

  /* Get current time */
  h = hourFormat12();  // Returns the hour of current time between 1-12
  m = minute();        // Returns the minute of current time
  s = second();        // Returns the second of current time (not used, included for completeness)

  Serial.print(h);
  Serial.print(":");
  Serial.print(m);
  Serial.print(":");
  Serial.println(s);

  /* Parse time values to light corresponding pixels */
  // individualPixels[0] = 1; //Light "IT"
  // individualPixels[1] = 1; //Light "IS"
  individualPixels[13] = 1;  //Light "時"
  pwm.setPWM(13, 0, UP);
  individualPixels[34] = 1;  //Light "分"
  pwm3.setPWM(2, 0, UP);


  // 零時
  if (h == 0) {
    individualPixels[0] = 1;
    pwm.setPWM(0, 0, UP);
  }
  // 壹時
  if (h == 1) {
    individualPixels[1] = 1;
    pwm.setPWM(1, 0, UP);
  }


  if (h == 2) {
    pwm.setPWM(1, 0, DOWN);
    individualPixels[2] = 1;
    pwm.setPWM(2, 0, UP);
  }


  if (h == 3) {
    pwm.setPWM(2, 0, DOWN);
    individualPixels[3] = 1;
    pwm.setPWM(3, 0, UP);
  }


  if (h == 4) {
    pwm.setPWM(3, 0, DOWN);
    individualPixels[4] = 1;
    pwm.setPWM(4, 0, UP);
  }

  if (h == 5) {
    pwm.setPWM(4, 0, DOWN);
    individualPixels[5] = 1;
    pwm.setPWM(5, 0, UP);
  }


  if (h == 6) {
    pwm.setPWM(5, 0, DOWN);
    individualPixels[6] = 1;
    pwm.setPWM(6, 0, UP);
  }

  if (h == 7) {
    pwm.setPWM(6, 0, DOWN);
    individualPixels[7] = 1;
    pwm.setPWM(7, 0, UP);
  }


  if (h == 8) {
    pwm.setPWM(7, 0, DOWN);
    individualPixels[8] = 1;
    pwm.setPWM(8, 0, UP);
  }

  if (h == 9) {
    pwm.setPWM(8, 0, DOWN);
    individualPixels[9] = 1;
    pwm.setPWM(9, 0, UP);
  }

  if (h == 10) {
    pwm.setPWM(9, 0, DOWN);
    individualPixels[10] = 1;
    pwm.setPWM(10, 0, UP);
  }


  if (h == 11) {
    individualPixels[10] = 1;
    individualPixels[11] = 1;
    pwm.setPWM(11, 0, UP);
  }

  if (h == 12) {
    pwm.setPWM(11, 0, DOWN);
    individualPixels[10] = 1;
    individualPixels[12] = 1;
    pwm.setPWM(10, 0, UP);
    pwm.setPWM(12, 0, UP);
  }

  //END of hour



  //START of minutes 零壹分
  if (m == 0) {
    pwm2.setPWM(8, 0, DOWN);
    pwm2.setPWM(3, 0, DOWN);
    pwm3.setPWM(1, 0, DOWN);
    individualPixels[14] = 1;
    pwm.setPWM(14, 0, UP);
  }

  if (m == 1) {
    pwm2.setPWM(8, 0, DOWN);
    individualPixels[14] = 1;
    individualPixels[15] = 1;
    pwm.setPWM(14, 0, UP);
    pwm.setPWM(15, 0, UP);
  }

  if (m == 2) {
    pwm.setPWM(15, 0, DOWN);
    individualPixels[14] = 1;
    individualPixels[16] = 1;
    pwm.setPWM(14, 0, UP);
    pwm2.setPWM(0, 0, UP);
  }


  if (m == 3) {
    pwm2.setPWM(0, 0, DOWN);
    individualPixels[14] = 1;
    individualPixels[17] = 1;
    pwm.setPWM(14, 0, UP);
    pwm2.setPWM(1, 0, UP);
  }

  if (m == 4) {
    pwm2.setPWM(1, 0, DOWN);
    individualPixels[14] = 1;
    individualPixels[18] = 1;
    pwm.setPWM(14, 0, UP);
    pwm2.setPWM(2, 0, UP);
  }

  if (m == 5) {
    pwm2.setPWM(2, 0, DOWN);
    individualPixels[14] = 1;
    individualPixels[19] = 1;
    pwm.setPWM(14, 0, UP);
    pwm2.setPWM(3, 0, UP);
  }

  if (m == 6) {
    pwm2.setPWM(3, 0, DOWN);
    individualPixels[14] = 1;
    individualPixels[20] = 1;
    pwm.setPWM(14, 0, UP);
    pwm2.setPWM(4, 0, UP);
  }

  if (m == 7) {
    pwm2.setPWM(4, 0, DOWN);
    individualPixels[14] = 1;
    individualPixels[21] = 1;
    pwm.setPWM(14, 0, UP);
    pwm2.setPWM(5, 0, UP);
  }

  if (m == 8) {
    pwm2.setPWM(5, 0, DOWN);
    individualPixels[14] = 1;
    individualPixels[22] = 1;
    pwm.setPWM(14, 0, UP);
    pwm2.setPWM(6, 0, UP);
  }

  if (m == 9) {
    pwm2.setPWM(6, 0, DOWN);
    individualPixels[14] = 1;
    individualPixels[23] = 1;
    pwm.setPWM(14, 0, UP);
    pwm2.setPWM(7, 0, UP);
  }

  if (m == 10) {
    pwm.setPWM(14, 0, DOWN);
    pwm2.setPWM(7, 0, DOWN);
    individualPixels[24] = 1;
    pwm2.setPWM(8, 0, UP);
  }


  if (m == 11) {
    individualPixels[24] = 1;
    individualPixels[25] = 1;
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(9, 0, UP);
  }

  if (m == 12) {
    pwm2.setPWM(9, 0, DOWN);
    individualPixels[24] = 1;
    individualPixels[26] = 1;
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(10, 0, UP);
  }

  if (m == 13) {
    pwm2.setPWM(10, 0, DOWN);
    individualPixels[24] = 1;
    individualPixels[27] = 1;
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(11, 0, UP);
  }

  if (m == 14) {
    pwm2.setPWM(11, 0, DOWN);
    individualPixels[24] = 1;
    individualPixels[28] = 1;
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(12, 0, UP);
  }

  if (m == 15) {
    pwm2.setPWM(12, 0, DOWN);
    individualPixels[24] = 1;
    individualPixels[29] = 1;
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(13, 0, UP);
  }

  if (m == 16) {
    pwm2.setPWM(13, 0, DOWN);
    individualPixels[24] = 1;
    individualPixels[30] = 1;
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(14, 0, UP);
  }

  if (m == 17) {
    pwm2.setPWM(14, 0, DOWN);
    individualPixels[24] = 1;
    individualPixels[31] = 1;
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(15, 0, UP);
  }

  if (m == 18) {
    pwm2.setPWM(15, 0, DOWN);
    individualPixels[24] = 1;
    individualPixels[32] = 1;
    pwm2.setPWM(8, 0, UP);
    pwm3.setPWM(0, 0, UP);
  }

  if (m == 19) {
    pwm3.setPWM(0, 0, DOWN);
    individualPixels[24] = 1;
    individualPixels[33] = 1;
    pwm2.setPWM(8, 0, UP);
    pwm3.setPWM(1, 0, UP);
  }
  //20

  if (m == 20) {
    pwm3.setPWM(1, 0, DOWN);
    individualPixels[16] = 1;
    individualPixels[24] = 1;
    pwm2.setPWM(0, 0, UP);
    pwm2.setPWM(8, 0, UP);
  }

  if (m == 21) {
    individualPixels[16] = 1;
    individualPixels[24] = 1;
    individualPixels[25] = 1;
    pwm2.setPWM(0, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(9, 0, UP);
  }

  if (m == 22) {
    pwm2.setPWM(9, 0, DOWN);
    individualPixels[16] = 1;
    individualPixels[24] = 1;
    individualPixels[26] = 1;
    pwm2.setPWM(0, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(10, 0, UP);
  }

  if (m == 23) {
    pwm2.setPWM(10, 0, DOWN);
    individualPixels[16] = 1;
    individualPixels[24] = 1;
    individualPixels[27] = 1;
    pwm2.setPWM(0, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(11, 0, UP);
  }

  if (m == 24) {
    pwm2.setPWM(11, 0, DOWN);
    individualPixels[16] = 1;
    individualPixels[24] = 1;
    individualPixels[28] = 1;
    pwm2.setPWM(0, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(12, 0, UP);
  }

  if (m == 25) {
    pwm2.setPWM(12, 0, DOWN);
    individualPixels[16] = 1;
    individualPixels[24] = 1;
    individualPixels[29] = 1;
    pwm2.setPWM(0, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(13, 0, UP);
  }

  if (m == 26) {
    pwm2.setPWM(13, 0, DOWN);
    individualPixels[16] = 1;
    individualPixels[24] = 1;
    individualPixels[30] = 1;
    pwm2.setPWM(0, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(14, 0, UP);
  }

  if (m == 27) {
    pwm2.setPWM(14, 0, DOWN);
    individualPixels[16] = 1;
    individualPixels[24] = 1;
    individualPixels[31] = 1;
    pwm2.setPWM(0, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(15, 0, UP);
  }

  if (m == 28) {
    pwm2.setPWM(15, 0, DOWN);
    individualPixels[16] = 1;
    individualPixels[24] = 1;
    individualPixels[32] = 1;
    pwm2.setPWM(0, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm3.setPWM(0, 0, UP);
  }

  if (m == 29) {
    pwm3.setPWM(0, 0, DOWN);
    individualPixels[16] = 1;
    individualPixels[24] = 1;
    individualPixels[33] = 1;
    pwm2.setPWM(0, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm3.setPWM(1, 0, UP);
  }

  //30
  if (m == 30) {
    pwm2.setPWM(0, 0, DOWN);
    pwm3.setPWM(1, 0, DOWN);
    individualPixels[17] = 1;
    individualPixels[24] = 1;
    pwm2.setPWM(1, 0, UP);
    pwm2.setPWM(8, 0, UP);
  }

  if (m == 31) {
    individualPixels[17] = 1;
    individualPixels[24] = 1;
    individualPixels[25] = 1;
    pwm2.setPWM(1, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(9, 0, UP);
  }

  if (m == 32) {
    pwm2.setPWM(9, 0, DOWN);
    individualPixels[17] = 1;
    individualPixels[24] = 1;
    individualPixels[26] = 1;
    pwm2.setPWM(1, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(10, 0, UP);
  }

  if (m == 33) {
    pwm2.setPWM(10, 0, DOWN);
    individualPixels[17] = 1;
    individualPixels[24] = 1;
    individualPixels[27] = 1;
    pwm2.setPWM(1, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(11, 0, UP);
  }

  if (m == 34) {
    pwm2.setPWM(11, 0, DOWN);
    individualPixels[17] = 1;
    individualPixels[24] = 1;
    individualPixels[28] = 1;
    pwm2.setPWM(1, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(12, 0, UP);
  }

  if (m == 35) {
    pwm2.setPWM(12, 0, DOWN);
    individualPixels[17] = 1;
    individualPixels[24] = 1;
    individualPixels[29] = 1;
    pwm2.setPWM(1, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(13, 0, UP);
  }

  if (m == 36) {
    pwm2.setPWM(13, 0, DOWN);
    individualPixels[17] = 1;
    individualPixels[24] = 1;
    individualPixels[30] = 1;
    pwm2.setPWM(1, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(14, 0, UP);
  }

  if (m == 37) {
    pwm2.setPWM(14, 0, DOWN);
    individualPixels[17] = 1;
    individualPixels[24] = 1;
    individualPixels[31] = 1;
    pwm2.setPWM(1, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(15, 0, UP);
  }

  if (m == 38) {
    pwm2.setPWM(15, 0, DOWN);
    individualPixels[17] = 1;
    individualPixels[24] = 1;
    individualPixels[32] = 1;
    pwm2.setPWM(1, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm3.setPWM(0, 0, UP);
  }

  if (m == 39) {
    pwm3.setPWM(0, 0, DOWN);
    individualPixels[17] = 1;
    individualPixels[24] = 1;
    individualPixels[33] = 1;
    pwm2.setPWM(1, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm3.setPWM(1, 0, UP);
  }

  //40
  if (m == 40) {
    pwm2.setPWM(1, 0, DOWN);
    pwm3.setPWM(1, 0, DOWN);
    individualPixels[18] = 1;
    individualPixels[24] = 1;
    pwm2.setPWM(2, 0, UP);
    pwm2.setPWM(8, 0, UP);
  }

  if (m == 41) {
    individualPixels[18] = 1;
    individualPixels[24] = 1;
    individualPixels[25] = 1;
    pwm2.setPWM(2, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(9, 0, UP);
  }

  if (m == 42) {
    pwm2.setPWM(9, 0, DOWN);
    individualPixels[18] = 1;
    individualPixels[24] = 1;
    individualPixels[26] = 1;
    pwm2.setPWM(2, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(10, 0, UP);
  }

  if (m == 43) {
    pwm2.setPWM(10, 0, DOWN);
    individualPixels[18] = 1;
    individualPixels[24] = 1;
    individualPixels[27] = 1;
    pwm2.setPWM(2, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(11, 0, UP);
  }

  if (m == 44) {
    pwm2.setPWM(11, 0, DOWN);
    individualPixels[18] = 1;
    individualPixels[24] = 1;
    individualPixels[28] = 1;
    pwm2.setPWM(2, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(12, 0, UP);
  }

  if (m == 45) {
    pwm2.setPWM(12, 0, DOWN);
    individualPixels[18] = 1;
    individualPixels[24] = 1;
    individualPixels[29] = 1;
    pwm2.setPWM(2, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(13, 0, UP);
  }

  if (m == 46) {
    pwm2.setPWM(13, 0, DOWN);
    individualPixels[18] = 1;
    individualPixels[24] = 1;
    individualPixels[30] = 1;
    pwm2.setPWM(2, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(14, 0, UP);
  }

  if (m == 47) {
    pwm2.setPWM(14, 0, DOWN);
    individualPixels[18] = 1;
    individualPixels[24] = 1;
    individualPixels[31] = 1;
    pwm2.setPWM(2, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(15, 0, UP);
  }

  if (m == 48) {
    pwm2.setPWM(15, 0, DOWN);
    individualPixels[18] = 1;
    individualPixels[24] = 1;
    individualPixels[32] = 1;
    pwm2.setPWM(2, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm3.setPWM(0, 0, UP);
  }

  if (m == 49) {
    pwm3.setPWM(0, 0, DOWN);
    individualPixels[18] = 1;
    individualPixels[24] = 1;
    individualPixels[33] = 1;
    pwm2.setPWM(2, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm3.setPWM(1, 0, UP);
  }

  //50
  if (m == 50) {
    pwm2.setPWM(2, 0, DOWN);
    pwm3.setPWM(1, 0, DOWN);
    individualPixels[19] = 1;
    individualPixels[24] = 1;
    pwm2.setPWM(3, 0, UP);
    pwm2.setPWM(8, 0, UP);
  }

  if (m == 51) {
    individualPixels[19] = 1;
    individualPixels[24] = 1;
    individualPixels[25] = 1;
    pwm2.setPWM(3, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(9, 0, UP);
  }

  if (m == 52) {
    pwm2.setPWM(9, 0, DOWN);
    individualPixels[19] = 1;
    individualPixels[24] = 1;
    individualPixels[26] = 1;
    pwm2.setPWM(3, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(10, 0, UP);
  }

  if (m == 53) {
    pwm2.setPWM(10, 0, DOWN);
    individualPixels[19] = 1;
    individualPixels[24] = 1;
    individualPixels[27] = 1;
    pwm2.setPWM(3, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(11, 0, UP);
  }

  if (m == 54) {
    pwm2.setPWM(11, 0, DOWN);
    individualPixels[19] = 1;
    individualPixels[24] = 1;
    individualPixels[28] = 1;
    pwm2.setPWM(3, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(12, 0, UP);
  }

  if (m == 55) {
    pwm2.setPWM(12, 0, DOWN);
    individualPixels[19] = 1;
    individualPixels[24] = 1;
    individualPixels[29] = 1;
    pwm2.setPWM(3, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(13, 0, UP);
  }

  if (m == 56) {
    pwm2.setPWM(13, 0, DOWN);
    individualPixels[19] = 1;
    individualPixels[24] = 1;
    individualPixels[30] = 1;
    pwm2.setPWM(3, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(14, 0, UP);
  }

  if (m == 57) {
    pwm2.setPWM(14, 0, DOWN);
    individualPixels[19] = 1;
    individualPixels[24] = 1;
    individualPixels[31] = 1;
    pwm2.setPWM(3, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm2.setPWM(15, 0, UP);
  }

  if (m == 58) {
    pwm2.setPWM(15, 0, DOWN);
    individualPixels[19] = 1;
    individualPixels[24] = 1;
    individualPixels[32] = 1;
    pwm2.setPWM(3, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm3.setPWM(0, 0, UP);
  }

  if (m == 59) {
    pwm3.setPWM(0, 0, DOWN);
    individualPixels[19] = 1;
    individualPixels[24] = 1;
    individualPixels[33] = 1;
    pwm2.setPWM(3, 0, UP);
    pwm2.setPWM(8, 0, UP);
    pwm3.setPWM(1, 0, UP);
  }

  /* Light pixels corresponding to current time */
  for (int i = 0; i < sizeof(individualPixels); i++) {
    if (individualPixels[i] == 1) {
      pixels.setPixelColor(i, pixels.Color(red, green, blue));  //Set Neopixel color
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }

  pixels.show();  //Display Neopixel color
}
