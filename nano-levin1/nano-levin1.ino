

/*
 * Copyright (c) 2015 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Seeed
 * Modified Time: July 2015
 * Description: Connect the IR receiver pins to D2 for this demo. You can see the remote control's infrared data 
 *         that received through a serial port terminal, then write the received infrared data into send.ino 
 *        and downloaded to the board with Infrared Emitter Grove, so you can send the same data with 
 *        remote control's button.
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <IRSendRev.h>

#include <Servo.h>


#define BIT_LEN         0
#define BIT_START_H     1
#define BIT_START_L     2
#define BIT_DATA_H      3
#define BIT_DATA_L      4
#define BIT_DATA_LEN    5
#define BIT_DATA        6

Servo servo;  // create servo object to control a servo



// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

const int PIN_RGB[]={3,4,5};
const int PIN_LED_RYBG[]={8,9,10,11};
const int PIN_LIGHT=A3;
const int PIN_FIRE=A4;
const int PIN_SMOKE=A5;
const int PIN_HUMAN=A0;
const int PIN_TONE=0;
const int PIN_BUTTON=10;
const int PIN_FADE=12;
const int PIN_IR=9;
const int PIN_IR_LED=13;
const int PIN_RELAY=5;
const int PIN_SERVO=7;
const int PIN_BL=8;

const int SERVO_MAX_L_ANGLE=140;
const int SERVO_MAX_R_ANGLE=50;
const int SERVO_STEP_ANGLE=5;

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
int ledState = HIGH;  

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

int layState=LOW;

int servo_angle=90;

void setup()
{
    //tone_molody(1);
   
    Serial.begin(9600);
    init_ir();
    init_rgb();
    init_led();
    init_fire();
    init_smoke();
    init_relay();
    //init_servo();
    init_button();
    //pinMode(PIN_TONE,OUTPUT); 
    //led_all_on();
    init_bl();
    //tone_alert(10000);
    delay(2000);
     rgb_off();
     //tone_off(); 
}

unsigned char dta[20];

void loop()
{
     int l=readLightLevel();
  //onLightLevelRead(l);
  l=readFireLevel();
  onFireLevelRead(l);
  l=readSmokeLevel();
  onSmokeLevelRead(l);
  //l=readHumanLevel();
  //onHumanLevelRead(l);
  l=readBLLevel();
  onBLLevelRead(l);
  l = digitalRead(PIN_BUTTON);
  onButtonPress(l);

  onFade();
   if(IR.IsDta())                  // get IR data
    {
        
        onIRRecv();
        digitalWrite(PIN_IR_LED, HIGH);
    }else{
        digitalWrite(PIN_IR_LED, LOW);  
    }
    delay(120);
}


void onFade(){

   analogWrite(PIN_FADE, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness == 0 || brightness == 155) {
    fadeAmount = -fadeAmount ;
  }
  
}

void onIRRecv(){
     IR.Recv(dta);               // receive data to dta

        Serial.println("+------------------------------------------------------+");
   Serial.print("LEN = ");
        Serial.println(dta[BIT_LEN]);
        Serial.print("START_H: ");
        Serial.print(dta[BIT_START_H]);
        Serial.print("\tSTART_L: ");
        Serial.println(dta[BIT_START_L]);
        
        Serial.print("DATA_H: ");
        Serial.print(dta[BIT_DATA_H]);
        Serial.print("\tDATA_L: ");
        Serial.println(dta[BIT_DATA_L]);
        
        Serial.print("\r\nDATA_LEN = ");
        Serial.println(dta[BIT_DATA_LEN]);
        
    Serial.print("DATA: ");
        for(int i=0; i<dta[BIT_DATA_LEN]; i++)
        {
            Serial.print("0x");
            Serial.print(dta[i+BIT_DATA], HEX);
            Serial.print("\t");
        }
        Serial.println();
    
    Serial.print("DATA: ");
        for(int i=0; i<dta[BIT_DATA_LEN]; i++)
        {
            Serial.print(dta[i+BIT_DATA], DEC);
            Serial.print("\t");
        }
        Serial.println();
        Serial.println("+------------------------------------------------------+\r\n\r\n");
   int dta2=dta[BIT_DATA+2],dta3=dta[BIT_DATA+3];
   if(dta2==162&&dta3==93){
      //power
      //init(); 
       rgb_off();
       led_all_off();
       digitalWrite(PIN_RELAY,LOW);
       servo.write(90);
   }
   else if(dta2==104&&dta3==151){
      //0
      rgb_off();
   }
   else if(dta2==48&&dta3==207){
      //1
      rgb_green();
   }else if(dta2==24&&dta3==231){
      //2
      rgb_blue();
   }else if(dta2==122&&dta3==133){
      //3
      rgb_yellow();
   }else if(dta2==16&&dta3==239){
      //4
      rgb_red();
   }else if(dta2==56&&dta3==199){
      //5
      rgb_purple();
   }else if(dta2==56&&dta3==199){
      //5
      rgb_purple();
   }
   else if(dta2==90&&dta3==165){
      //6
      rgb_white();
    }else if(dta2==66&&dta3==189){
      //7
      rgb_white();
    }else if(dta2==74&&dta3==181){
      //8
      rgb_white();
    }else if(dta2==82&&dta3==173){
      //9
      rgb_white();
    }
   else if(dta2==226&&dta3==29){
      //sound off
      tone_off();
    }
    else if(dta2==224&&dta3==31){
      //eq
      layState=!layState;
      digitalWrite(PIN_RELAY,layState);
    }else if(dta2==168&&dta3==87){
      //vol-
      servo_trun_left(SERVO_STEP_ANGLE);
    }else if(dta2==144&&dta3==111){
      //vol+
      servo_trun_right(SERVO_STEP_ANGLE);
    }else if(dta2==2&&dta3==253){
      //prev
      servo_trun_to(SERVO_MAX_L_ANGLE);
      
    }else if(dta2==194&&dta3==61){
      //next
      servo_trun_to(SERVO_MAX_R_ANGLE);
      
    }
    else if(dta2==152&&dta3==103){
      //rpt
      servo_trun_to(90);
      
    }
    else if(dta2==176&&dta3==79){
      //scan
      servo_trun_to(SERVO_MAX_L_ANGLE);
      delay(2000);
       servo_trun_to(SERVO_MAX_R_ANGLE);
      delay(2000);
      servo_trun_to(90);
    }
    else if(dta2==98&&dta3==157){
      //mode
    }
    else if(dta2==98&&dta3==157){
      //mode
    }else if(dta2==34&&dta3==221){
      //play
    }
         
}

void onButtonPress(int reading ){

   
  Serial.println(reading);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        rgb_white();
      }else{
         rgb_off();  
      }
    }
  }
  
  // set the LED:
  //digitalWrite(PIN_RGB[0], ledState);
  //digitalWrite(PIN_RGB[2], ledState);
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
  
}


void onLightLevelRead(int level){
      String info="LightLevel:";
      info+=String(level);
      Serial.println(info);
      int s=LOW;
      if(level>2){
        s=HIGH;
      }
      digitalWrite(PIN_LED_RYBG[2],s);
}

void onFireLevelRead(int level){
      String info="FireLevel:";
      info+=String(level);
      Serial.println(info);
      int s=LOW;
      if(level>1100){
        s=HIGH;
        //play_sound(1);
      }
      //digitalWrite(PIN_LED_RYBG[0],s);
}

void onSmokeLevelRead(int level){
      
      String info="SmokeLevel:";
      info+=String(level);
      Serial.println(info);
      int s=LOW;
      if(level>100){
        s=HIGH;
        //play_sound(1);
        //tone_alert(100);
      }else{
        //tone_off();  
      }
      //digitalWrite(PIN_LED_RYBG[3],s);
      
}


void onHumanLevelRead(int level){
      
      String info="HumanLevel:";
      info+=String(level);
      //Serial.println(info);
      int s=LOW;
      if(level>512){
        s=HIGH;
      }
      digitalWrite(PIN_LED_RYBG[1],s);
      
}


void onBLLevelRead(int level){
      
      String info="BL:";
      info+=String(level);
      Serial.println(info);
      int s=LOW;
      if(level==0){
        s=HIGH;
      }
      //digitalWrite(PIN_IR_LED, HIGH);
      digitalWrite(PIN_IR_LED,s);
      delay(1000);
}


int readLightLevel(){
    return analogReadLevel(PIN_LIGHT);    
}

int readFireLevel(){

    return analogRead(PIN_FIRE);//analogReadLevel(PIN_FIRE);    

}

int readBLLevel(){

    return digitalRead(PIN_BL);//analogReadLevel(PIN_FIRE);    

}

int readSmokeLevel(){

    return analogRead(PIN_SMOKE);
   

}

int readHumanLevel(){

    int level=analogRead(PIN_HUMAN);
    return level;    

}

int analogReadLevel(int pin)
{

    unsigned int vol_t = 0;
    for(int i=0; i<32; i++)
    {
        vol_t += analogRead(pin);
    }
    vol_t = vol_t>>5;
    vol_t = map(vol_t, 0, 1023, 0, 10);
    return vol_t;

}





void init_rgb(){
   pinMode(PIN_RGB[0], OUTPUT);
   pinMode(PIN_RGB[1], OUTPUT);
   pinMode(PIN_RGB[2], OUTPUT);
}

void init_led(){
    
      pinMode(PIN_FADE, OUTPUT);
   //pinMode(PIN_LED_RYBG[1], OUTPUT);
   //pinMode(PIN_LED_RYBG[2], OUTPUT);
   //pinMode(PIN_LED_RYBG[3], OUTPUT);
}

void init_fire(){
      pinMode(PIN_FIRE, INPUT);
    
}

void init_smoke(){
      pinMode(PIN_SMOKE, INPUT);
}

void init_bl(){
     pinMode(PIN_BL,INPUT);  
}

void init_button(){
      pinMode(PIN_BUTTON,INPUT);  
      digitalWrite(PIN_BUTTON,LOW);
}

void init_tone(){
    pinMode(PIN_TONE,OUTPUT);  
}

void init_relay(){
    pinMode(PIN_RELAY,OUTPUT);  
}

void init_ir(){

  IR.Init(PIN_IR);
  pinMode(PIN_IR_LED,OUTPUT);   
  
}

void init_servo(){
   servo.attach(PIN_SERVO); 
   servo.write(90); 
}

void led_all_off(){
    digitalWrite(PIN_LED_RYBG[0],LOW);
   digitalWrite(PIN_LED_RYBG[1],LOW);
   digitalWrite(PIN_LED_RYBG[2],LOW);
   digitalWrite(PIN_LED_RYBG[3],LOW);
}

void led_all_on(){
    digitalWrite(PIN_LED_RYBG[0],HIGH);
   digitalWrite(PIN_LED_RYBG[1],HIGH);
   digitalWrite(PIN_LED_RYBG[2],HIGH);
   digitalWrite(PIN_LED_RYBG[3],HIGH);
}

void rgb_white(){

   digitalWrite(PIN_RGB[0],LOW);
   digitalWrite(PIN_RGB[1],LOW);
   digitalWrite(PIN_RGB[2],LOW);
}

void rgb_blue(){

   digitalWrite(PIN_RGB[0],LOW);
   digitalWrite(PIN_RGB[1],HIGH);
   digitalWrite(PIN_RGB[2],HIGH);
}

void rgb_green(){

   digitalWrite(PIN_RGB[0],HIGH);
   digitalWrite(PIN_RGB[1],LOW);
   digitalWrite(PIN_RGB[2],HIGH);
}

void rgb_yellow(){

   digitalWrite(PIN_RGB[0],HIGH);
   digitalWrite(PIN_RGB[1],LOW);
   digitalWrite(PIN_RGB[2],LOW);
}

void rgb_red(){

   digitalWrite(PIN_RGB[0],HIGH);
   digitalWrite(PIN_RGB[1],HIGH);
   digitalWrite(PIN_RGB[2],LOW);
}

void rgb_purple(){

   digitalWrite(PIN_RGB[0],LOW);
   digitalWrite(PIN_RGB[1],HIGH);
   digitalWrite(PIN_RGB[2],LOW);
}

void rgb_x(){

   digitalWrite(PIN_RGB[0],LOW);
   digitalWrite(PIN_RGB[1],HIGH);
   digitalWrite(PIN_RGB[2],LOW);
}

void rgb_off(){
  digitalWrite(PIN_RGB[0],HIGH);
   digitalWrite(PIN_RGB[1],HIGH);
   digitalWrite(PIN_RGB[2],HIGH);
}

void tone_on(){
    digitalWrite(PIN_TONE,HIGH);//鍙戝０闊�
}

void tone_off(){
    digitalWrite(PIN_TONE,LOW);
}

void tone_alert(int times){
   
}

void servo_trun_left(int angle){
     if(servo_angle<SERVO_MAX_L_ANGLE){
         servo_angle+=angle;
     }
     servo_trun_to(servo_angle);
     delay(15);
}

void servo_trun_right(int angle){
     if(servo_angle>SERVO_MAX_R_ANGLE){
         servo_angle-=angle;
     }
     servo_trun_to(servo_angle);
     delay(15);
}

void servo_trun_to(int angle){
     servo.write(angle);
}

void tone_molody(int times){
  for(int i=0;i<times;i++){
// iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    //tone(PIN_TONE, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    //tone_off();
    }
  }
}

