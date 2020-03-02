#include <Wire.h>
#include "DS1307.h"
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"

Adafruit_LiquidCrystal lcd(0);
DS1307 clock;

#define encoder 3

int motor1 = 13;
int motor2 = 12;

int feed_time;
int feed_time_pot = A0;
int feed_time_pot_last;

int feed_length;
int feed_length_pot = A1;
int feed_length_pot_last;

int time_hour;
int time_hour_pot = A2;
int time_hour_pot_last;

int time_minute;
int time_minute_pot = A3;
int time_minute_pot_last;

volatile unsigned int currentRotation = 0; //Current rotations counted by encoder
int currentRotation_a;

void setup()
{
  Serial.begin(9600);
  clock.begin();
  clock.fillByHMS(11, 59, 50); //15:28 30"
  clock.setTime();//write time to the RTC chip
  lcd.begin(16, 2);
  pinMode(encoder, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), pulse, RISING); //Count rotations of encoder
}

void loop() {
  //printTime();
  feed_time = analogRead(feed_time_pot) / 42;
  feed_length = analogRead(feed_length_pot) / 11;

  time_hour = analogRead(time_hour_pot) / 42;
  time_minute = analogRead(time_minute_pot) / 17.06;


  clock.getTime();
  if (clock.second == 0) {
    setAll();
  }

  if (feed_time != feed_time_pot_last) {
    setAll();
    feed_time_pot_last = feed_time;
  }
  
  if(feed_length != feed_length_pot_last) {
    setAll();
    feed_length_pot_last = feed_length;
  }

  if (clock.hour == feed_time && clock.second == 5) {
    delay(1000);
    feed();
  }
  //Serial.println(currentRotation);

  if (time_hour != time_hour_pot_last || time_minute != time_minute_pot_last) {
    setNewTime();
    time_hour_pot_last = time_hour;
    time_minute_pot_last = time_minute;
  }
}

void feed() {
  Serial.println("FEEDING");
  do {
    motor(0);
    lcd.begin(16, 2);
    lcd.print(currentRotation / 60);
    lcd.print("/");
    lcd.print(feed_length);
  } while (currentRotation / 60 < feed_length);
  Serial.println("FEEDING STOPPED");
  motor(2);
  setAll();
}

void setAll() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(feed_time);
  lcd.print("   ");
  lcd.print(clock.hour);
  lcd.print(":");
  if (clock.minute < 10) {
    lcd.print("0");
  }
  lcd.print(clock.minute);
  lcd.setCursor(0, 1);
  lcd.print("Rotations: ");
  lcd.print(feed_length);
}

void setFeedTime() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(feed_time);
}

void setRotations() {
  lcd.print("Rotations: ");
  lcd.print(feed_length);
}

void setCurrentTime() {
  lcd.print(clock.hour);
  lcd.print(":");
  if (clock.minute < 10) {
    lcd.print("0");
  }
  lcd.print(clock.minute);
}

void printTime()
{
  clock.getTime();
  Serial.print(clock.hour, DEC);
  Serial.print(":");
  Serial.print(clock.minute, DEC);
  Serial.print(":");
  Serial.print(clock.second, DEC);
  Serial.println(" ");
}

void setNewTime() {
  clock.fillByHMS(time_hour, time_minute, 0);
  clock.setTime();
  setAll();
}

void motor(int mode) {
  if (mode == 0) { //Spin motor forward
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, HIGH);
  }
  if (mode == 1) { //Spin motor backward
    digitalWrite(motor1, HIGH);
    digitalWrite(motor2, LOW);
  }
  if (mode == 2) { //Turn motor off
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
  }
}

void pulse() {
  currentRotation++;
}
