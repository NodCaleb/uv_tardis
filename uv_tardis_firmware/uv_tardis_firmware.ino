#include "GyverTM1637.h"
#define output_pin 2
#define rt_clk_pin 3
#define rt_dt_pin 4
#define rt_sw_pin 5
#define disp_clk_pin 6
#define disp_dio_pin 7
#define active_pin 8

int rotation;
int value;
bool skip = false;

int seconds = 0;
int new_seconds = 0;

GyverTM1637 disp(disp_clk_pin, disp_dio_pin);

void setup() {
  Serial.begin (9600);
  pinMode(output_pin, OUTPUT);
  pinMode (rt_clk_pin,INPUT);
  pinMode (rt_dt_pin,INPUT);
  pinMode (rt_sw_pin,INPUT);
  pinMode (active_pin,INPUT);
  rotation = digitalRead(rt_clk_pin);

  disp.clear();
  disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)
  displayTime(seconds);
}

void loop() {

  bool active = !digitalRead(active_pin); //Closing cirquit will ground the active_pin

  if (active && seconds > 0){
    digitalWrite(output_pin, 1);
    new_seconds--;
    delay(1000);
  }
  else{
    digitalWrite(output_pin, 0);
  }
  
  if (!active){    
    value = digitalRead(rt_clk_pin);
     if (value != rotation){ // we use the DT pin to find out which way we turning.

      if(skip){ //Skipping every other signal, cause that fucking selector sends 2 in 1 click
        skip = false;
      }
      else{
        skip = true;
        if (digitalRead(rt_dt_pin) != value) {  // Clockwise
          new_seconds += secondsStep(seconds);
        } else { //Counterclockwise
          new_seconds -= secondsStep(seconds-1);
          if(new_seconds < 0) new_seconds = 0;
        }
      }          
   } 
   rotation = value;

   if(!digitalRead(rt_sw_pin)){
     new_seconds = 0;
   }    
  }

  if (seconds != new_seconds){
    seconds = new_seconds;
    displayTime(seconds);
  }

  
   
}

int secondsStep(int value){
  if (value < 0) return 0; //To prevent negative values
  if (value < 30) return 5;
  if (value < 120) return 10;
  if (value < 300) return 30;
  return 60;
}

void displayTime(int value){
  int minutesToDisplay = value / 60;
  int secondsToDisplay = value % 60;

  int digit0 = minutesToDisplay / 10;
  int digit1 = minutesToDisplay % 10;
  int digit2 = secondsToDisplay / 10;
  int digit3 = secondsToDisplay % 10;

  disp.clear();
  disp.display(digit0, digit1, digit2, digit3);
  disp.point(true);
}
