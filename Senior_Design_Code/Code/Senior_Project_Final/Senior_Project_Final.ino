#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
// pins for spi connection
U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);
uint8_t draw_state = 0;

// constants won't change. They're used here to set pin numbers:
const int IR1Pin = 2;
const int IR2Pin = 3;
const int IR3Pin = 9;
const int IR4Pin = 8;
const int IR5Pin = 8;  
//const int ledPin = 3;    // the number of the LED pin
const int buttonPin = 12;
const int fanPin = 30;
int state = LOW;
int buttonState = LOW;
// variables will change:
int IRState1 = 0;  // variable for reading the IR status
int IRState2 = 0;  // variable for reading the IR status
int IRState3 = 0;  // variable for reading the IR status
int IRState4 = 0;  // variable for reading the IR status
int IRState5 = 0;  // variable for reading the IR status
int IRTotal = 0;
int flag = 62;
int startFlag = 0;
int systemState = 0;
int drawState = 0;
// data sent to jetson via i2c
int data = 0;
int results[4];
int num = 5;
int temp;
int index = 0;
int i2cAddress = 0x55;
// how many response to optic
int howMany;
float acc = 0;

int latchPin = 5;
int clockPin = 6;
int dataPin = 4;
int latchPin1 = 45;
int clockPin1 = 46;
int dataPin1 = 47;
const int ledPin9 = 22;
const int ledPin10 = 23;
const int ledPin11 = 25;
const int ledPin12 = 27;

byte leds = 0;
byte leds1 = 0;

void setup() {
  // i2c setup
  Wire.begin(0x55);
  Wire.onRequest(sendData);
  Wire.onReceive(receiveData);

  Serial.begin(9600);
  // initialize the LED pin as an output:
  //pinMode(ledPin, OUTPUT);
  // initialize the IR sensor pin as an input:
  pinMode(IR1Pin, INPUT);
  pinMode(IR2Pin, INPUT);
  pinMode(IR3Pin, INPUT);
  pinMode(IR4Pin, INPUT);
  pinMode(IR5Pin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin1, OUTPUT);
  pinMode(dataPin1, OUTPUT);  
  pinMode(clockPin1, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(ledPin9, OUTPUT);
  pinMode(ledPin10, OUTPUT);
  pinMode(ledPin11, OUTPUT);
  pinMode(ledPin12, OUTPUT);
  // start lcd
  u8g2.begin();
  Serial.begin(9600);
}

// ************* i2c communication *************
// i2c send data
void sendData(){
  Wire.write(data);
}

// i2c reieve data event
void receiveData(int num){
  while(Wire.available()){
    temp = Wire.read();
    if(temp != 1 && index < 5){
      results[index] = temp;
      index++;
    }
    else if(temp == 1 && results[index - 1] != 1){
      results[index] = temp;
      index++;
    }
    else if(index >= 5){
      index = 0;
    }
  }
  results[1] = 5;
  results[2] = 9;
  results[3] = 7;
  acc = (results[1] * 10) + results[2] + (results[3] * 0.1);
  /*if(results[4] > 0){
    howMany = results[4];
    if(howMany == 1){
      u8g2.drawCircle(51,28,3); // box (2,1)
    }
    else if(howMany == 2){
      u8g2.drawCircle(51,28,3); // box (2,1)
      u8g2.drawCircle(65,28,3); // box (2,2)
    }
    else if(howMany == 3){
      u8g2.drawCircle(51,28,3); // box (2,1)
      u8g2.drawCircle(65,28,3); // box (2,2)
      u8g2.drawCircle(79,28,3);// box (2,3)
    }
  }
  else{
    u8g2.drawStr(50,23,"X"); // box (2,1)
    u8g2.drawStr(64,23,"X"); // box (2,2)
    u8g2.drawStr(78,23,"X");// box (2,3)
  }
  */
}

// **********  LCD  *************
// lcd setup
void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}
/*
void u8g2_startup(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(5, 10, "Please press the button to start.");
}
void drawStart(void) {
  u8g2_prepare();
  u8g2_startup(draw_state&7);
}
*/
// draw lcd layout
void u8g2_setup(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.setCursor(60, 41);
  u8g2.print(acc);
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);
  
  /* //hits
  u8g2.drawCircle(51,17,3); // box (1,1)
  u8g2.drawCircle(65,17,3); // box (1,2)
  u8g2.drawCircle(79,17,3);// box (1,3)
  u8g2.drawCircle(92,17,3);// box (1,4)
  u8g2.drawCircle(106,17,3);// box (1,5)

  u8g2.drawCircle(51,28,3); // box (2,1)
  u8g2.drawCircle(65,28,3); // box (2,2)
  u8g2.drawCircle(79,28,3);// box (2,3)
  u8g2.drawCircle(92,28,3);// box (2,4)
  u8g2.drawCircle(106,28,3);// box (2,5)
  */
  /*
  // no hits
  u8g2.drawStr(50,13,"X"); // box (1,1)
  u8g2.drawStr(64,13,"X"); // box (1,2)
  u8g2.drawStr(78,13,"X");// box (1,3)
  u8g2.drawStr(92,13,"X");// box (1,4)
  u8g2.drawStr(106,13,"X");// box (1,5)

  u8g2.drawStr(50,23,"X"); // box (2,1)
  u8g2.drawStr(64,23,"X"); // box (2,2)
  u8g2.drawStr(78,23,"X");// box (2,3)
  u8g2.drawStr(92,23,"X");// box (2,4)
  u8g2.drawStr(106,23,"X");// box (2,5)
  */
}
// all no hits
void u8g2_0(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);

  // no hits
  u8g2.drawStr(50,13,"X"); // box (1,1)
  u8g2.drawStr(64,13,"X"); // box (1,2)
  u8g2.drawStr(78,13,"X");// box (1,3)

  u8g2.drawStr(50,23,"X"); // box (2,1)
  u8g2.drawStr(64,23,"X"); // box (2,2)
  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
void u8g2_1(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);

  u8g2.drawCircle(51,17,3); // box (1,1) hit
  u8g2.drawStr(64,13,"X"); // box (1,2)
  u8g2.drawStr(78,13,"X");// box (1,3)

  u8g2.drawStr(50,23,"X"); // box (2,1)
  u8g2.drawStr(64,23,"X"); // box (2,2)
  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
void u8g2_2(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);

  u8g2.drawCircle(51,17,3); // box (1,1) hit
  u8g2.drawCircle(65,17,3); // box (1,2) hit
  u8g2.drawStr(78,13,"X");// box (1,3)

  u8g2.drawStr(50,23,"X"); // box (2,1)
  u8g2.drawStr(64,23,"X"); // box (2,2)
  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
void u8g2_3(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);

  u8g2.drawCircle(51,17,3); // box (1,1) hit
  u8g2.drawCircle(65,17,3); // box (1,2) hit
  u8g2.drawCircle(79,17,3);// box (1,3) hit

  u8g2.drawStr(50,23,"X"); // box (2,1)
  u8g2.drawStr(64,23,"X"); // box (2,2)
  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
void u8g2_4(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);

  u8g2.drawStr(50,13,"X"); // box (1,1)
  u8g2.drawStr(64,13,"X"); // box (1,2)
  u8g2.drawStr(78,13,"X");// box (1,3)

  u8g2.drawCircle(51,28,3); // box (2,1) hit
  u8g2.drawStr(64,23,"X"); // box (2,2)
  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
void u8g2_5(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);

  u8g2.drawStr(50,13,"X"); // box (1,1)
  u8g2.drawStr(64,13,"X"); // box (1,2)
  u8g2.drawStr(78,13,"X");// box (1,3)

  u8g2.drawCircle(51,28,3); // box (2,1) hit
  u8g2.drawCircle(65,28,3); // box (2,2) hit
  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
void u8g2_6(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);

  u8g2.drawStr(50,13,"X"); // box (1,1)
  u8g2.drawStr(64,13,"X"); // box (1,2)
  u8g2.drawStr(78,13,"X");// box (1,3)

  u8g2.drawCircle(51,28,3); // box (2,1) hit
  u8g2.drawCircle(65,28,3); // box (2,2) hit
  u8g2.drawCircle(79,28,3);// box (2,3) hit

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
// draw state = 8
void u8g2_1x1(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);
  
 //hits
  u8g2.drawCircle(51,17,3); // box (1,1)

  u8g2.drawCircle(51,28,3); // box (2,1)
  // no hits

  u8g2.drawStr(64,13,"X"); // box (1,2)
  u8g2.drawStr(78,13,"X");// box (1,3)

  u8g2.drawStr(64,23,"X"); // box (2,2)
  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
// 9 2x1
void u8g2_2x1(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);
  
 //hits
  u8g2.drawCircle(51,17,3); // box (1,1)
  u8g2.drawCircle(65,17,3); // box (1,2)

  u8g2.drawCircle(51,28,3); // box (2,1)
  // no hits

  u8g2.drawStr(78,13,"X");// box (1,3)

  u8g2.drawStr(64,23,"X"); // box (2,2)
  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
// 10 3x1
void u8g2_3x1(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);
  
 //hits
  u8g2.drawCircle(51,17,3); // box (1,1)
  u8g2.drawCircle(65,17,3); // box (1,2)
  u8g2.drawCircle(79,17,3);// box (1,3) hit

  u8g2.drawCircle(51,28,3); // box (2,1)
  // no hits

  u8g2.drawStr(64,23,"X"); // box (2,2)
  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
// 11 3x2
void u8g2_3x2(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);
  
 //hits
  u8g2.drawCircle(51,17,3); // box (1,1)
  u8g2.drawCircle(65,17,3); // box (1,2)
  u8g2.drawCircle(79,17,3);// box (1,3) hit

  u8g2.drawCircle(51,28,3); // box (2,1)
  u8g2.drawCircle(65,28,3); // box (2,2) hit
  // no hits

  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
// 12 3x3
void u8g2_3x3(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);
  
 //hits
  u8g2.drawCircle(51,17,3); // box (1,1)
  u8g2.drawCircle(65,17,3); // box (1,2)
  u8g2.drawCircle(79,17,3);// box (1,3) hit

  u8g2.drawCircle(51,28,3); // box (2,1)
  u8g2.drawCircle(65,28,3); // box (2,2) hit
  u8g2.drawCircle(79,28,3);// box (2,3) hit

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
// 13 2x3
void u8g2_2x3(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);
  
 //hits
  u8g2.drawCircle(51,17,3); // box (1,1)
  u8g2.drawCircle(65,17,3); // box (1,2)

  u8g2.drawCircle(51,28,3); // box (2,1)
  u8g2.drawCircle(65,28,3); // box (2,2)
  u8g2.drawCircle(79,28,3);// box (2,3)
  // no
  u8g2.drawStr(78,13,"X");// box (1,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
// 14 1x2
void u8g2_1x2(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);
  

  u8g2.drawCircle(51,17,3); // box (1,1)
  
  u8g2.drawCircle(51,28,3); // box (2,1)
  u8g2.drawCircle(65,28,3); // box (2,2)
 
  u8g2.drawStr(64,13,"X"); // box (1,2)
  u8g2.drawStr(78,13,"X");// box (1,3)

  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
// 15 2x2
void u8g2_2x2(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);
  
  //hits
  u8g2.drawCircle(51,17,3); // box (1,1)
  u8g2.drawCircle(65,17,3); // box (1,2)
  //u8g2.drawCircle(79,17,3);// box (1,3)

  u8g2.drawCircle(51,28,3); // box (2,1)
  u8g2.drawCircle(65,28,3); // box (2,2)
  //u8g2.drawCircle(79,28,3);// box (2,3)
  // no hits
  //u8g2.drawStr(50,13,"X"); // box (1,1)
  //u8g2.drawStr(64,13,"X"); // box (1,2)
  u8g2.drawStr(78,13,"X");// box (1,3)

  //u8g2.drawStr(50,23,"X"); // box (2,1)
  //u8g2.drawStr(64,23,"X"); // box (2,2)
  u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}
// 16 1x3
void u8g2_1x3(uint8_t a) {
  // draw string (x,y,"string")
  u8g2.drawStr(7, 1, "System");
  u8g2.drawStr(49, 1, "1");
  u8g2.drawStr(63, 1, "2");
  u8g2.drawStr(77, 1, "3");
  u8g2.drawStr(91, 1, "4");
  u8g2.drawStr(105, 1, "5");
  u8g2.drawStr(11, 13, "IR");
  u8g2.drawStr(11, 23, "Optic");
  //u8g2.drawStr(5, 54, "Response Time:");
  u8g2.drawStr(5, 40, "Accuracy:");
  // draw frame (x,y,width,height)
  // frame around words 
  // IR
  u8g2.drawFrame(9,11,36,12);
  // Optic
  u8g2.drawFrame(9,22,36,12);
  // box one (IR)
  u8g2.drawFrame(44,11,15,12);
  // box two
  u8g2.drawFrame(58,11,15,12);
  // box three
  u8g2.drawFrame(72,11,15,12);
  // box four
  u8g2.drawFrame(86,11,15,12);
  // box five
  u8g2.drawFrame(100,11,15,12);
  // box one (Optic)
  u8g2.drawFrame(44,22,15,12);
  // box two
  u8g2.drawFrame(58,22,15,12);
  // box three
  u8g2.drawFrame(72,22,15,12);
  // box four
  u8g2.drawFrame(86,22,15,12);
  // box five
  u8g2.drawFrame(100,22,15,12);
  
  //hits
  u8g2.drawCircle(51,17,3); // box (1,1)
  //u8g2.drawCircle(65,17,3); // box (1,2)
  //u8g2.drawCircle(79,17,3);// box (1,3)

  u8g2.drawCircle(51,28,3); // box (2,1)
  u8g2.drawCircle(65,28,3); // box (2,2)
  u8g2.drawCircle(79,28,3);// box (2,3)
  // no hits
  //u8g2.drawStr(50,13,"X"); // box (1,1)
  u8g2.drawStr(64,13,"X"); // box (1,2)
  u8g2.drawStr(78,13,"X");// box (1,3)

  //u8g2.drawStr(50,23,"X"); // box (2,1)
  //u8g2.drawStr(64,23,"X"); // box (2,2)
  //u8g2.drawStr(78,23,"X");// box (2,3)

  u8g2.setCursor(60, 41);
  u8g2.print(acc);
}

// function to call draw layout
void draw(void) {
  u8g2_prepare();
  if(drawState == 0){
    u8g2_setup(draw_state&7);
  }
  // no hits
  else if(drawState == 1){
    u8g2_0(draw_state&7);
  }
  // 1 IR
  else if(drawState == 2){
    u8g2_1(draw_state&7);
  }
  // 2 IR
  else if(drawState == 3){
    u8g2_2(draw_state&7);
  }
  // 3 IR
  else if(drawState == 4){
    u8g2_3(draw_state&7);
  }
  // 1 Optic
  else if(drawState == 5){
    u8g2_4(draw_state&7);
  }
  // 2 Optic
  else if(drawState == 6){
    u8g2_5(draw_state&7);
  }
  // 3 Optic
  else if(drawState == 7){
    u8g2_6(draw_state&7);
  }
  else if(drawState == 8){
    u8g2_1x1(draw_state&7);
  }
  else if(drawState == 9){
    u8g2_2x1(draw_state&7);
  }
  else if(drawState == 10){
    u8g2_3x1(draw_state&7);
  }
  else if(drawState == 11){
    u8g2_3x2(draw_state&7);
  }
  else if(drawState == 12){
    u8g2_3x3(draw_state&7);
  }
  else if(drawState == 13){
    u8g2_2x3(draw_state&7);
  }
  else if(drawState == 14){
    u8g2_1x2(draw_state&7);
  }
  else if(drawState == 15){
    u8g2_2x2(draw_state&7);
  }
  else if(drawState == 16){
    u8g2_1x3(draw_state&7);
  }
}

// ********* IR Sensor **********
void runSensor(){
    // read the state of the pushbutton value:
  IRState1 = digitalRead(IR1Pin);
  IRState2 = digitalRead(IR2Pin);
  IRState3 = digitalRead(IR3Pin);
  IRState4 = digitalRead(IR4Pin);
  IRState5 = digitalRead(IR5Pin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (IRState1 == HIGH && IRState2 == LOW && IRState3 == LOW) {
    // turn LED on:
    LEDON1();
    LEDOFF2();
    // put a circle in the first IR box
    //u8g2.drawCircle(51,17,3); // box (1,1)
    drawState = 2;
    u8g2.firstPage();  
    do {
      draw();
    } while( u8g2.nextPage() );
    delay(100);
    IRTotal = 1;

  } 
  else if(IRState1 == HIGH && IRState2 == HIGH && IRState3 == LOW) {
    // turn LED off:
    LEDON1();
    LEDOFF2();
    LEDON3();
    LEDOFF4();
    drawState = 3;
    u8g2.firstPage();  
    do {
      draw();
    } while( u8g2.nextPage() );
    delay(100);
    IRTotal = 2;

  }
  else if(IRState1 == HIGH && IRState2 == HIGH && IRState3 == HIGH) {
    // turn LED off:
    LEDON1();
    LEDOFF2();
    LEDON3();
    LEDOFF4();
    LEDON5();
    LEDOFF6();
    drawState = 4;
    u8g2.firstPage();  
    do {
      draw();
    } while( u8g2.nextPage() );
    delay(100);
    IRTotal = 3;
  }
  else if (IRState1 == LOW && IRState2 == HIGH && IRState3 == LOW) {
    // turn LED on:
    LEDON3();
    LEDOFF4();
    drawState = 2;
    u8g2.firstPage();  
    do {
      draw();
    } while( u8g2.nextPage() );
    delay(100);
    IRTotal = 1;

  } 
  else if(IRState1 == LOW && IRState2 == HIGH && IRState3 == HIGH) {
    // turn LED off:
    LEDON3();
    LEDOFF4();
    LEDON5();
    LEDOFF6();
    drawState = 3;
    u8g2.firstPage();  
    do {
      draw();
    } while( u8g2.nextPage() );
    delay(100);
    IRTotal = 2;
  }
  else if(IRState3 == HIGH) {
    // turn LED off:
    LEDON5();
    LEDOFF6();
    drawState = 2;
    u8g2.firstPage();  
    do {
      draw();
    } while( u8g2.nextPage() );
    delay(100);
    IRTotal = 1;
  }
}

// ********** LED Functions *********
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, MSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}
void updateShiftRegister1()
{
   digitalWrite(latchPin1, LOW);
   shiftOut(dataPin1, clockPin1, MSBFIRST, leds1);
   digitalWrite(latchPin1, HIGH);
}
// turn off all leds
void LEDOFFALL()
{
  leds = 0;
  updateShiftRegister();
  delay(200);

}
// turn off the second led
void LEDOFF2()
{
  bitClear(leds, 1);
  updateShiftRegister();
  delay(200);

}
void LEDOFF1()
{
  bitClear(leds, 0);
  updateShiftRegister();
  delay(200);

}
void LEDOFF3()
{
  bitClear(leds, 2);
  updateShiftRegister();
  delay(200);

}
void LEDOFF4()
{
  bitClear(leds, 3);
  updateShiftRegister();
  delay(200);
}
void LEDOFF5()
{
  bitClear(leds, 4);
  updateShiftRegister();
  delay(200);

}
void LEDOFF6()
{
  bitClear(leds, 5);
  updateShiftRegister();
  delay(200);

}

void LEDOFF7()
{
  bitClear(leds, 6);
  updateShiftRegister();
  delay(200);

}
void LEDOFF8()
{
  bitClear(leds, 7);
  updateShiftRegister();
  delay(200);

}
void LEDOFF9()
{
  digitalWrite(ledPin9, LOW);
  //bitClear(leds1, 1);
  //updateShiftRegister1();
  //delay(200);

}
void LEDOFF10()
{
  digitalWrite(ledPin10, HIGH);
  //bitClear(leds1, 2);
  //updateShiftRegister1();
  //delay(200);
}
void LEDOFF11()
{
  digitalWrite(ledPin11, LOW);
  //bitClear(leds1, 3);
  //updateShiftRegister1();
  //delay(200);

}
void LEDOFF12()
{
  digitalWrite(ledPin12, LOW);
  //bitClear(leds1, 4);
  //updateShiftRegister1();
  //delay(200);

}

// turn on LEDS
void LEDON2()
{
  bitSet(leds, 1);
  updateShiftRegister();
  delay(200);

}
void LEDON1()
{
  bitSet(leds, 0);
  updateShiftRegister();
  delay(200);

}
void LEDON3()
{
  bitSet(leds, 2);
  updateShiftRegister();
  delay(200);

}
void LEDON4()
{
  bitSet(leds, 3);
  updateShiftRegister();
  delay(200);
}
void LEDON5()
{
  bitSet(leds, 4);
  updateShiftRegister();
  delay(200);

}
void LEDON6()
{
  bitSet(leds, 5);
  updateShiftRegister();
  delay(200);

}
void LEDON7()
{
  bitSet(leds, 6);
  updateShiftRegister();
  delay(200);

}
void LEDON8()
{
  bitSet(leds, 7);
  updateShiftRegister();
  delay(200);

}
void LEDON9()
{
  digitalWrite(ledPin9, HIGH);
  //bitSet(leds1, 0);
  //updateShiftRegister1();
  //delay(200);

}
void LEDON10()
{
  digitalWrite(ledPin10, HIGH);
  //bitSet(leds1, 1);
  //updateShiftRegister1();
  //delay(200);
}
void LEDON11()
{
  digitalWrite(ledPin11, HIGH);
  //bitSet(leds1, 2);
  //updateShiftRegister1();
  //delay(200);

}
void LEDON12()
{
  digitalWrite(ledPin12, HIGH);
  //bitSet(leds1, 3);
  //updateShiftRegister1();
  //delay(200);

}

void loop(void) {
  buttonState = digitalRead(buttonPin);
  //flag = 59;
  if(startFlag == 0){
    // turn on red leds
    LEDON2();
    LEDON4();
    LEDON6();
    LEDON8();
    LEDON9();
    LEDON11();
    digitalWrite(fanPin, LOW);
    drawState = 0;
    u8g2.firstPage();  
    do {
      draw();
    } while( u8g2.nextPage() );
    // set up a flag for when response is receieved from systems, then use a switch/if like above
    delay(150);
    startFlag++;
  }
  else if(buttonState == HIGH && flag > 60){
    Serial.println("Button Pressed");
    Serial.println(results[1]);
    Serial.println(results[2]);
    Serial.println(results[3]);
    Serial.println(results[4]);
    LEDON2();
    LEDON4();
    LEDON6();
    LEDOFF1();
    LEDOFF3();
    LEDOFF5();
    delay(100);
    flag = 0;
    Wire.beginTransmission(i2cAddress);
    data = 1;
    sendData();
    Wire.endTransmission();
    systemState = HIGH;
  }
  else if(flag < 60 && systemState == HIGH){
    Serial.println("Running Sensor");
    runSensor();
    Wire.beginTransmission(i2cAddress);
    data = 0;
    sendData();
    Wire.endTransmission();
    flag++;
  }
  else if(flag == 60){
    Serial.println("Running LCD data.");
    systemState = LOW;
    if(results[4] > 0){
      howMany = results[4];
      if(howMany == 1){
        LEDON7();
        LEDOFF8();
        if(IRTotal == 1){
          drawState = 8;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
        else if(IRTotal == 2){
          drawState = 9;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
        else if(IRTotal == 3){
          drawState = 10;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
        else{
          drawState = 5;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
      }
      else if(howMany == 2){
        LEDON7();
        LEDOFF8();
        LEDOFF9();
        LEDON10();
        if(IRTotal == 1){
          drawState = 14;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
        else if(IRTotal == 2){
          drawState = 15;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
        else if(IRTotal == 3){
          drawState = 11;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
        else{
          drawState = 6;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
      }
      else if(howMany == 3){
        LEDON7();
        LEDOFF8();
        LEDOFF9();
        LEDON10();
        LEDOFF11();
        LEDON12();
        if(IRTotal == 1){
          drawState = 16;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
        else if(IRTotal == 2){
          drawState = 13;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
        else if (IRTotal == 3){
          drawState = 12;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
        else{
          drawState = 7;
          u8g2.firstPage();  
          do {
            draw();
          } while( u8g2.nextPage() );
          delay(100);
        }
      }
    }
    flag++;
  }
}