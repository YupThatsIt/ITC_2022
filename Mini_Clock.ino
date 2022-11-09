#include "LedControl.h"
#include <EEPROM.h>

#define CNG_TIME_BTN_PIN 5
#define CNG_DIG_BTN_PIN 6
#define CNG_MODE_PIN 3
#define EEPROM_PIN 4
#define BUZZER_PIN 2
#define GRAVITY_PIN A1

#define DIN_PIN 11
#define CLK_PIN 13
#define CS_PIN 10

#define BUFFER_SIZE 16

int timer1_counter;

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 4); // DIN,CLK,CS,Number of LED Module

//Variables for debounce
struct ButtonVar{
  uint8_t pin;
  uint8_t buttonState;
  uint8_t lastButtonState = HIGH;
  int debounceDelay = 20;
  unsigned long lastDebounceTime = 0;
  bool isPress = false;
  bool isProceed = false;
};

struct Time{
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

struct ButtonVar ctSwitch, cdSwitch, mdSwitch, svSwitch;
struct Time clk, alm;

uint8_t lastMin;
uint8_t lastDig[4];
uint8_t digits[4];

//Display LED on 32 x 8 coord system
void plot(uint8_t x, uint8_t y, uint8_t value){
  uint8_t address;
  if (x >= 0 && x <= 7){ address = 3; }
  else if (x >= 8 && x <= 15){ address = 2; x -= 8; }
  else if (x >= 16 && x <= 23){ address = 1; x -= 16; }
  else if (x >= 24 && x <= 31){ address = 0; x -= 24; }

  if (value == 1){ lc.setLed(address, y, x, true); }
  else { lc.setLed(address, y, x, false); }
}

//diplay selected number on selected digit (1 - 4) with invert functionality
void display_dig(uint8_t dig, uint8_t num, uint8_t isInvert){
  int8_t start, invertx = 0, inverty = 0;
  if (isInvert == 1){ invertx = -31; inverty = -7; }
  
  if (dig == 1) { start = 3; }
  else if (dig == 2) { start = 9; }
  else if (dig == 3) { start = 19; }
  else if (dig == 4) { start = 25; }
  
  switch(num){
  case 0: 
    plot(abs(invertx + start), abs(inverty + 2), 1);
    plot(abs(invertx + start), abs(inverty + 3), 1);
    plot(abs(invertx + start), abs(inverty + 4), 1);
    plot(abs(invertx + start), abs(inverty + 5), 1);
    plot(abs(invertx + start + 1), abs(inverty + 1), 1);
    plot(abs(invertx + start + 1), abs(inverty + 6), 1);
    plot(abs(invertx + start + 2), abs(inverty + 1), 1);
    plot(abs(invertx + start + 2), abs(inverty + 6), 1);
    plot(abs(invertx + start + 3), abs(inverty + 2), 1);
    plot(abs(invertx + start + 3), abs(inverty + 3), 1);
    plot(abs(invertx + start + 3), abs(inverty + 4), 1);
    plot(abs(invertx + start + 3), abs(inverty + 5), 1);
    break;
  case 1: 
    plot(abs(invertx + start), abs(inverty + 6), 1);
    plot(abs(invertx + start + 1), abs(inverty + 2), 1);
    plot(abs(invertx + start + 1), abs(inverty + 6), 1);
    plot(abs(invertx + start + 2), abs(inverty + 1), 1);
    plot(abs(invertx + start + 2), abs(inverty + 2), 1);
    plot(abs(invertx + start + 2), abs(inverty + 3), 1);
    plot(abs(invertx + start + 2), abs(inverty + 4), 1);
    plot(abs(invertx + start + 2), abs(inverty + 5), 1);
    plot(abs(invertx + start + 2), abs(inverty + 6), 1);
    plot(abs(invertx + start + 3), abs(inverty + 6), 1);
    break;
  case 2: 
    plot(abs(invertx + start), abs(inverty + 2), 1);
    plot(abs(invertx + start), abs(inverty + 6), 1);
    plot(abs(invertx + start + 1), abs(inverty + 1), 1);
    plot(abs(invertx + start + 1), abs(inverty + 5), 1);
    plot(abs(invertx + start + 1), abs(inverty + 6), 1);
    plot(abs(invertx + start + 2), abs(inverty + 1), 1);
    plot(abs(invertx + start + 2), abs(inverty + 4), 1);
    plot(abs(invertx + start + 2), abs(inverty + 6), 1);
    plot(abs(invertx + start + 3), abs(inverty + 2), 1);
    plot(abs(invertx + start + 3), abs(inverty + 3), 1);
    plot(abs(invertx + start + 3), abs(inverty + 6), 1);
    break;
  case 3: 
    plot(abs(invertx + start), abs(inverty + 1), 1);
    plot(abs(invertx + start), abs(inverty + 5), 1);
    plot(abs(invertx + start + 1), abs(inverty + 1), 1);
    plot(abs(invertx + start + 1), abs(inverty + 6), 1);
    plot(abs(invertx + start + 2), abs(inverty + 1), 1);
    plot(abs(invertx + start + 2), abs(inverty + 3), 1);
    plot(abs(invertx + start + 2), abs(inverty + 6), 1);
    plot(abs(invertx + start + 3), abs(inverty + 1), 1);
    plot(abs(invertx + start + 3), abs(inverty + 2), 1);
    plot(abs(invertx + start + 3), abs(inverty + 4), 1);
    plot(abs(invertx + start + 3), abs(inverty + 5), 1);
    break;
  case 4: 
    plot(abs(invertx + start), abs(inverty + 3), 1);
    plot(abs(invertx + start), abs(inverty + 4), 1);
    plot(abs(invertx + start), abs(inverty + 5), 1);
    plot(abs(invertx + start + 1), abs(inverty + 2), 1);
    plot(abs(invertx + start + 1), abs(inverty + 5), 1);
    plot(abs(invertx + start + 2), abs(inverty + 1), 1);
    plot(abs(invertx + start + 2), abs(inverty + 2), 1);
    plot(abs(invertx + start + 2), abs(inverty + 3), 1);
    plot(abs(invertx + start + 2), abs(inverty + 4), 1);
    plot(abs(invertx + start + 2), abs(inverty + 5), 1);
    plot(abs(invertx + start + 2), abs(inverty + 6), 1);
    plot(abs(invertx + start + 3), abs(inverty + 5), 1);
    break;
  case 5: 
    plot(abs(invertx + start), abs(inverty + 1), 1);
    plot(abs(invertx + start), abs(inverty + 2), 1);
    plot(abs(invertx + start), abs(inverty + 3), 1);
    plot(abs(invertx + start), abs(inverty + 5), 1);
    plot(abs(invertx + start + 1), abs(inverty + 1), 1);
    plot(abs(invertx + start + 1), abs(inverty + 3), 1);
    plot(abs(invertx + start + 1), abs(inverty + 6), 1);
    plot(abs(invertx + start + 2), abs(inverty + 1), 1);
    plot(abs(invertx + start + 2), abs(inverty + 3), 1);
    plot(abs(invertx + start + 2), abs(inverty + 6), 1);
    plot(abs(invertx + start + 3), abs(inverty + 1), 1);
    plot(abs(invertx + start + 3), abs(inverty + 4), 1);
    plot(abs(invertx + start + 3), abs(inverty + 5), 1);
    break;
  case 6: 
    plot(abs(invertx + start), abs(inverty + 2), 1);
    plot(abs(invertx + start), abs(inverty + 3), 1);
    plot(abs(invertx + start), abs(inverty + 4), 1);
    plot(abs(invertx + start), abs(inverty + 5), 1);
    plot(abs(invertx + start + 1), abs(inverty + 1), 1);
    plot(abs(invertx + start + 1), abs(inverty + 3), 1);
    plot(abs(invertx + start + 1), abs(inverty + 6), 1);
    plot(abs(invertx + start + 2), abs(inverty + 1), 1);
    plot(abs(invertx + start + 2), abs(inverty + 3), 1);
    plot(abs(invertx + start + 2), abs(inverty + 6), 1);
    plot(abs(invertx + start + 3), abs(inverty + 4), 1);
    plot(abs(invertx + start + 3), abs(inverty + 5), 1);
    break;
  case 7: 
    plot(abs(invertx + start), abs(inverty + 1), 1);
    plot(abs(invertx + start + 1), abs(inverty + 1), 1);
    plot(abs(invertx + start + 1), abs(inverty + 5), 1);
    plot(abs(invertx + start + 1), abs(inverty + 6), 1);
    plot(abs(invertx + start + 2), abs(inverty + 1), 1);
    plot(abs(invertx + start + 2), abs(inverty + 3), 1);
    plot(abs(invertx + start + 2), abs(inverty + 4), 1);
    plot(abs(invertx + start + 3), abs(inverty + 1), 1);
    plot(abs(invertx + start + 3), abs(inverty + 2), 1);
    break;
  case 8: 
    plot(abs(invertx + start), abs(inverty + 2), 1);
    plot(abs(invertx + start), abs(inverty + 4), 1);
    plot(abs(invertx + start), abs(inverty + 5), 1);
    plot(abs(invertx + start + 1), abs(inverty + 1), 1);
    plot(abs(invertx + start + 1), abs(inverty + 3), 1);
    plot(abs(invertx + start + 1), abs(inverty + 6), 1);
    plot(abs(invertx + start + 2), abs(inverty + 1), 1);
    plot(abs(invertx + start + 2), abs(inverty + 3), 1);
    plot(abs(invertx + start + 2), abs(inverty + 6), 1);
    plot(abs(invertx + start + 3), abs(inverty + 2), 1);
    plot(abs(invertx + start + 3), abs(inverty + 4), 1);
    plot(abs(invertx + start + 3), abs(inverty + 5), 1);
    break;
  case 9: 
    plot(abs(invertx + start), abs(inverty + 2), 1);
    plot(abs(invertx + start), abs(inverty + 3), 1);
    plot(abs(invertx + start + 1), abs(inverty + 1), 1);
    plot(abs(invertx + start + 1), abs(inverty + 4), 1);
    plot(abs(invertx + start + 1), abs(inverty + 6), 1);
    plot(abs(invertx + start + 2), abs(inverty + 1), 1);
    plot(abs(invertx + start + 2), abs(inverty + 4), 1);
    plot(abs(invertx + start + 2), abs(inverty + 6), 1);
    plot(abs(invertx + start + 3), abs(inverty + 2), 1);
    plot(abs(invertx + start + 3), abs(inverty + 3), 1);
    plot(abs(invertx + start + 3), abs(inverty + 4), 1);
    plot(abs(invertx + start + 3), abs(inverty + 5), 1);
    break;  
  }
}

void clear_dig(uint8_t dig, uint8_t isInvert){
  uint8_t start;
  if (isInvert == 1) {
    if (dig == 1) { start = 25; }
    if (dig == 2) { start = 19; }
    if (dig == 3) { start = 9; }
    if (dig == 4) { start = 3; }
  }
  else{
    if (dig == 1) { start = 3; }
    if (dig == 2) { start = 9; }
    if (dig == 3) { start = 19; }
    if (dig == 4) { start = 25; }
  }

  for (uint8_t y = 1; y <= 6; y++){
    for (uint8_t x = 0; x < 4; x++){
      plot(x + start, y, 0);
    }
  }
}

void change_dig(uint8_t dig, uint8_t value, uint8_t isInvert){
  clear_dig(dig, isInvert);
  display_dig(dig, value, isInvert);
}

void display_colon(){
  plot(15, 1, 1);
  plot(15, 2, 1);
  plot(15, 5, 1);
  plot(15, 6, 1);
  plot(16, 1, 1);
  plot(16, 2, 1);
  plot(16, 5, 1);
  plot(16, 6, 1);
}

void display_symbol(uint8_t value, uint8_t isInvert, char symbol){
  int8_t invertx = 0, inverty = 0;
  if (isInvert == 1){ invertx = -31; inverty = -7; }

  int wakeup[6][32] = {{1,0,0,0,1,0,0,1,1,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,0,1,0,1,1,1,0},
                       {1,0,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,0,0,0,0,0,1,0,0,1,0,1,0,0,1},
                       {1,0,1,0,1,0,1,0,0,1,0,1,1,0,0,0,1,1,1,1,0,0,0,1,0,0,1,0,1,0,0,1},
                       {1,0,1,0,1,0,1,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,1,0,0,1,0,1,1,1,0},
                       {1,0,1,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,1,0,0,1,0,1,0,0,0},
                       {0,1,0,1,0,0,1,0,0,1,0,1,0,0,1,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,0,0}};

  int saved[6][28] = {{0,1,1,0,0,0,0,1,1,0,0,0,1,0,0,1,0,0,1,1,1,1,0,0,1,1,1,0},
                      {1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1},
                      {1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1},
                      {0,1,1,0,0,0,1,1,1,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1},
                      {0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1},
                      {1,1,1,0,0,0,1,0,0,1,0,0,0,1,1,0,0,0,1,1,1,1,0,0,1,1,1,0}};

  if (symbol == 'c'){
    plot(abs(invertx + 30), abs(inverty + 5), value);
    plot(abs(invertx + 30), abs(inverty + 6), value);
    plot(abs(invertx + 30), abs(inverty + 7), value);
    plot(abs(invertx + 31), abs(inverty + 5), value);
    plot(abs(invertx + 31), abs(inverty + 7), value);
  }
  if (symbol == 'a'){
    for (uint8_t x = 0; x < 32; x++){
      for (uint8_t y = 1; y <= 6; y++){
        if (value == 1){ plot(abs(invertx + x), abs(inverty + y), wakeup[y - 1][x]); }
        else { plot(abs(invertx + x), abs(inverty + y), wakeup[y - 1][x] - 1);}
      }
    }
  }
  if (symbol == 's'){
    for (uint8_t x = 2; x < 30; x++){
      for (uint8_t y = 1; y <= 6; y++){
        if (value == 1){ plot(abs(invertx + x), abs(inverty + y), saved[y - 1][x - 2]); }
        else { plot(abs(invertx + x), abs(inverty + y), saved[y - 1][x] - 1);}
      }
    }
  }
}

void calculate_digits(Time timer){
  if (timer.minute < 10){
    digits[3] = timer.minute;
    digits[2] = 0;
  }
  else {
    digits[3] = timer.minute % 10;
    digits[2] = timer.minute / 10;
  }

  if (timer.hour < 10){
    digits[1] = timer.hour;
    digits[0] = 0;
  }
  else {
    digits[1] = timer.hour % 10;
    digits[0] = timer.hour / 10;
  }
}

int buff[BUFFER_SIZE];
int buff_pos = 0;

int get_gravity(){
  int sum = 0;
  buff[buff_pos] = analogRead(GRAVITY_PIN);
  buff_pos = (buff_pos + 1) % BUFFER_SIZE;
  for (uint8_t i = 0; i < BUFFER_SIZE; i++){
    sum += buff[i];
  }
  return round(sum / BUFFER_SIZE);
}

void debounce(ButtonVar *sw){
  sw->buttonState = digitalRead(sw->pin);
  //Serial.println(sw->buttonState);
  
  if (sw->buttonState != sw->lastButtonState){
    sw->lastDebounceTime = millis();
    sw->isPress = 1;
  }

  if (millis() - sw->lastDebounceTime > sw->debounceDelay){
    //Serial.println(sw->buttonState);
    if (sw->buttonState == LOW && sw->isPress == 1){
      sw->isProceed = 1;
      sw->isPress = 0;
    }
  }
  sw->lastButtonState = sw->buttonState;
}

unsigned long curTime;
bool isInvert;
bool lastInvert;
uint8_t curBlinkDig;
uint8_t mode;
bool isCounting;

void change_time(uint8_t *mode, Time *timer){ //Changing current timer
  if (millis() - curTime < 250){
    clear_dig(curBlinkDig, isInvert);
  }
  else if (millis() - curTime < 500){
    display_dig(curBlinkDig, digits[curBlinkDig - 1], isInvert);
  }
  else {
    curTime = millis();
  }

    //Switch Digit
  if (cdSwitch.isProceed){
    display_dig(curBlinkDig, digits[curBlinkDig - 1], isInvert);
    curBlinkDig++;
    if (curBlinkDig > 4){
      isCounting = true;
      curBlinkDig = 0;
      *mode = *mode - 2;
    }
    cdSwitch.isProceed = 0;
  }

  if (ctSwitch.isProceed){
    if (curBlinkDig == 1){
      timer->hour += 10;
      if (timer->hour > 30){
        timer->hour = 0 + timer->hour % 10;
      }
      else if (timer->hour >= 24 && timer->hour <= 30){
        timer->hour = 23;
      }
    }
    if (curBlinkDig == 2){
      timer->hour += 1;
      if (timer->hour >= 24){
        timer->hour -= 4;
      }
      else if (timer->hour % 10 == 0){
        timer->hour -= 10;
      }
    }
    if (curBlinkDig == 3){
      timer->minute += 10;
      if (timer->minute >= 60){
        timer->minute = 0 + timer->minute % 10;
      }
    }
    if (curBlinkDig == 4){
      timer->minute += 1;
      if (timer->minute % 10 == 0){
        timer->minute -= 10;
      }
    }
    clear_dig(curBlinkDig, isInvert);
    calculate_digits(*timer);
    ctSwitch.isProceed = 0;
  }
}

void check_alarm(){ //To check if alm and clk is identical or not
  if (clk.hour == alm.hour && clk.minute == alm.minute && clk.second == alm.second){
    tone(BUZZER_PIN, 250, 5000);
    Serial.println("WAKE UP!!!!!!!!!!!!!!");
    for (uint8_t y = 0; y < 7; y++){
      for (uint8_t x = 0; x < 31; x++){
        plot(x, y, 0);
      }
    }
    display_symbol(1, isInvert, 'a');
    mode = 5;
  }
}

ISR(TIMER1_OVF_vect){        // interrupt service routine 
  TCNT1 = timer1_counter;   // preload timer
  if (isCounting){
    clk.second += 1;
  }
  //Update time
  if (clk.second == 60){
    clk.minute++;
    clk.second = 0;
    if (clk.minute == 60){
      clk.hour++;
      clk.minute = 0;
      if (clk.hour == 24){
        clk.hour = 0;  
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  
  //Button Setup
  ctSwitch.pin = CNG_TIME_BTN_PIN;
  cdSwitch.pin = CNG_DIG_BTN_PIN;
  mdSwitch.pin = CNG_MODE_PIN;
  svSwitch.pin = EEPROM_PIN;
  pinMode(ctSwitch.pin, INPUT_PULLUP);
  pinMode(cdSwitch.pin, INPUT_PULLUP);
  pinMode(mdSwitch.pin, INPUT_PULLUP);
  pinMode(svSwitch.pin, INPUT_PULLUP);

  //Buzzer Pin Setup
  pinMode(BUZZER_PIN, OUTPUT);

  uint8_t devices = lc.getDeviceCount(); // find no of LED modules

  noInterrupts();            // disable all interrupts
  TCCR1A = 0;                // Set Mode of Timer : Normal Mode
  TCCR1B = 0;

  timer1_counter = 3036;     // preload timer 65536-16MHz/256/1Hz -> 62500 per second

  TCNT1 = timer1_counter;    // preload timer
  TCCR1B |= (1 << CS12);     // 256 prescaler
  TIMSK1 |= (1 << TOIE1);    // enable timer overflow interrupt
  interrupts();              // enable all interrupts

  //get stuff from EEPROM 
  int eeAddress = 0;
  EEPROM.get(eeAddress, clk);
  eeAddress += sizeof(clk);
  EEPROM.get(eeAddress, alm);

  lastMin = clk.minute;

  isInvert = 0;
  lastInvert = 0;
  mode = 1;
  isCounting = true;

   //we have to init all devices in a loop
  for(uint8_t address=0; address < devices; address++) { // set up each device 
    lc.shutdown(address, false);
    lc.setIntensity(address, 7);
    lc.clearDisplay(address);
  }

  //Initial Display
  calculate_digits(clk);
  for (uint8_t i = 0; i < 4; i++){
    change_dig(1 + i, digits[i], isInvert);
    lastDig[i] = digits[i];
  }
  display_colon();
}

void loop() { 
  //unsigned long curTime;
  int gravity = get_gravity();

  //Check gravity
  if (gravity >= 480){
    isInvert = 0;
  }
  else if (gravity <= 400){
    isInvert = 1;
  }

  if (mode == 1 || mode == 2 || mode == 5){
    debounce(&mdSwitch);
  }
  if (mode == 1 || mode == 2){
    debounce(&svSwitch);
  }
  debounce(&ctSwitch);
  debounce(&cdSwitch);

  //Check mode;
  if (mdSwitch.isProceed == 1 && mode == 1){
    mode = 2;
    calculate_digits(alm);
    for (uint8_t i = 0; i < 4; i++){
      change_dig(1 + i, digits[i], isInvert);
    }
    display_symbol(1, isInvert, 'c');
    mdSwitch.isProceed = 0;
  }
  else if (mdSwitch.isProceed == 1 && mode == 2){
    mode = 1;
    calculate_digits(clk);
    for (uint8_t i = 0; i < 4; i++){
      change_dig(1 + i, digits[i], isInvert);
    }
    display_symbol(0, isInvert, 'c');
    mdSwitch.isProceed = 0;
  }

  //To change time mode
  if (mode == 1){
    if (ctSwitch.isProceed == 1 || cdSwitch.isProceed == 1){
      isCounting = false;
      curBlinkDig = 1;
      mode = 3;
      ctSwitch.isProceed = 0, cdSwitch.isProceed = 0;
    }
  }
  if (mode == 2){
    if (ctSwitch.isProceed == 1 || cdSwitch.isProceed == 1){
      curBlinkDig = 1;
      mode = 4;
      ctSwitch.isProceed = 0, cdSwitch.isProceed = 0;
    }
  }

  //Change time mode
  if (mode == 3){
    change_time(&mode, &clk);
  }
  else if (mode == 4){
    change_time(&mode, &alm);
  }

  if (mode == 1){ //Check Alarm in mode 1
    check_alarm();
  }

  if (mode == 5 && mdSwitch.isProceed == 1){ //Alarm goes on in mode 5
    noTone(BUZZER_PIN);
    display_symbol(0, isInvert, 'a');
    display_colon();
    calculate_digits(clk);
    for (uint8_t i = 0; i < 4; i++){
      change_dig(1 + i, digits[i], isInvert);
    }
    mode = 1;
    mdSwitch.isProceed = 0;
  }

  if (svSwitch.isProceed == 1){
    int eeAddress = 0;
    EEPROM.put(eeAddress, clk);
    eeAddress += sizeof(clk);
    EEPROM.put(eeAddress, alm);
    for (uint8_t y = 0; y < 7; y++){
      for (uint8_t x = 0; x < 31; x++){
        plot(x, y, 0);
      }
    }
    display_symbol(1, lastInvert, 's');
    delay(1000);
    display_symbol(0, lastInvert, 's');
    for (uint8_t i = 0; i < 4; i++){
        change_dig(1 + i, digits[i], isInvert);
    }
    if (mode == 2){
      display_symbol(1, isInvert, 'c');
    }
    display_colon();
    svSwitch.isProceed = 0;
  }

  //Calculate each digits and change time accordingly
  if (lastMin != clk.minute && mode == 1){
    calculate_digits(clk);
    for (uint8_t i = 0; i < 4; i++){
      if (digits[i] != lastDig[i]){
        change_dig(1 + i, digits[i], isInvert);
      }
      lastDig[i] = digits[i];
    }
    lastMin = clk.minute;
  }
  
  //Update Invert
  if (isInvert != lastInvert){
    if (mode == 5){
      display_symbol(0, lastInvert, 'a');
      display_symbol(1, isInvert, 'a');
    }
    else {
      for (uint8_t i = 0; i < 4; i++){
        change_dig(1 + i, digits[i], isInvert);
      }
      if (mode == 2){
        display_symbol(0, lastInvert, 'c');
        display_symbol(1, isInvert, 'c');
      }
    }
    lastInvert = isInvert;  
  }

  Serial.print(clk.hour);
  Serial.print(":");
  Serial.print(clk.minute);
  Serial.print(":");
  Serial.println(clk.second);
}
