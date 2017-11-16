

#include "pitches.h"




const int PIN_TONE=3;
const int PIN_LATCH = 4;
const int PIN_CLOCK =7;
const int PIN_DATA = 8; //这里定义了那三个脚
const int PIN_BLS=9;
const int PIN_LM35=A4;
const int PIN_SMOKE=A5;
const int PIN_BTN0=5;
const int PIN_FF=6;


unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

unsigned char Dis_table[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90};  //LED状态显示的变量
unsigned char Dis_buf[]   = {0xF1,0xF2,0xF4,0xF8};
unsigned char disbuff[]  =  {0, 0, 0, 0};
int SUM_BLS = 0;


const long TEXI_TIME0=300000;
const long TEXI_TIME1=60000;
const int TEXI_AMOUNT1=1;

const int FF_LIMIT=100;

int texi_sum_amount=10;
int texi_mode=1;
long texi_timer0=0;
long texi_timer1=0;

int VAL_FF=0;

int Flag_up = 1;
int Flag_up1 = 1;

int lcd_delay=0;

int mode=2;

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};



void setup() {

  
  // put your setup code here, to run once:
   Serial.begin(9600);
 
    init_tone();
    init_ff();
    //tone_molody(1);
    init_texi();
    init_btn();
    tone_on();
    delay(1000);
    tone_off();
    init_bls();
    //init_led();
    //init_smoke();
    onModeChang(mode);
    init_display();
  
    //setNumber(0);
}

void display()
{
  for(char i=0; i<=3; i++)//扫描四个数码管
  {
    digitalWrite(PIN_LATCH,LOW); //将ST_CP口上面加低电平让芯片准备好接收数据
    shiftOut(PIN_DATA,PIN_CLOCK,MSBFIRST,Dis_table[disbuff[i]]); //发送显示码 0-3
    shiftOut(PIN_DATA,PIN_CLOCK,MSBFIRST,Dis_buf[i] );  //发送通值                          //串行数据输入引脚为dataPin，时钟引脚为clockPin，执行MSB有限发送，发送数据table[i]
    digitalWrite(PIN_LATCH,HIGH); //将ST_CP这个针脚恢复到高电平
    
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
   

   //thread1_entry(&thread1);
    int starttime = millis();
    onButtonPress();
    if(mode==1){
     //bls
       
       
               
    }
    
     else if(mode==2){
       display();
    }else  if(mode==3){ 
       if(onFFRead(digitalRead(PIN_FF),analogRead(A5))){
           tone_on();
           delay(10);
           tone_off();
       } 
    }
    delay(20);
    int looptime = millis() - starttime;
    if(onBLSRead(digitalRead(PIN_BLS))){
         tone_on();
         delay(1000);
         tone_off(); 
   
          onModeChang(mode);  
    }
    if(onTexiAmountCal( looptime)){

          setNumber(texi_sum_amount);
          onModeChang(mode);
    }
    
    display();
}





void onButtonPress(){
     if(digitalRead(A1)==LOW){
         mode=1; 
         onModeChang(mode);
         
         delay(500); 
     }
     if(digitalRead(A2)==LOW){
        mode=2;
         onModeChang(mode);
         delay(500);
     }
     if(digitalRead(A3)==LOW){
          mode=3; 
         onModeChang(mode);
         delay(500);
     } 
     if(digitalRead(PIN_BTN0)==HIGH){
         if(mode>=3){
           mode=1;
           }
         else 
          { 
            mode++;
            }
         onModeChang(mode);
         delay(500);
     }
     
  
}

void onModeChang(int m){
    
     if(m==1){
         setNumber(SUM_BLS);
     }else if(m==2){
         setNumber(texi_sum_amount);
     }else if(m==3){
         setNumber(VAL_FF);
     }
     //digitalWrite(m,LOW);
  
}

int onTexiAmountCal(int looptime){
       //int ms=millis();
       //int diff=ms-texi_timer0;
       
      
       if(texi_timer0<TEXI_TIME0){
        texi_timer0+=looptime;
         Serial.println(texi_timer0);
        return false;
       }else if(texi_mode==1){
          texi_sum_amount+=TEXI_AMOUNT1;
          texi_mode=2;
          tone_on();
          delay(200);
          tone_off();
          return true; 
       }

       //diff=ms-texi_timer1;
       
       texi_timer1+=looptime;

       Serial.println(texi_timer1);
       if(texi_timer1<TEXI_TIME1) return false;

       texi_timer1=0;
       texi_sum_amount+=TEXI_AMOUNT1;
       return true;
  
}

void onTempRead(float s){
   float c = (s - 32 - 459.67) / 1.8;
  //int temp = s * 0.48876;       //�转换数值 
  //Serial.print("LM35 = ");    
  //Serial.println(temp);       //��打印输出
  setNumber(c);
  //delay(1000);
 }

void onPM25Read(float r){
      float c = 1.1*pow(r,3)-3.8*pow(r,2)+520*r+0.62; // using spec sheet curve
      Serial.print(r);
    Serial.print(",");
    c=c/100;
    Serial.println(c);
    setNumber(r);
    lcd_delay=9000;
}

bool onBLSRead(int s){
    //Serial.println(s);
    if(s==0){
    SUM_BLS=SUM_BLS+1;
    setNumber(SUM_BLS); 
   
    return true;
    }
    return false;
    
}

void onSmokeLevelRead(int level){
      
      String info="SmokeLevel:";
      info+=String(level);
      Serial.println(info);
     // int s=LOW;
      if(level>100){
        //s=HIGH;
        //play_sound(1);
        //tone_alert(100);
        tone_molody(1);
      }
      //digitalWrite(PIN_LED_RYBG[3],s);
      
}

boolean onFFRead(int dval,int aval){
   // Serial.println(dval);
    //Serial.println(aval);
    setNumber(aval);
    return aval>=FF_LIMIT;
}

int readSmokeLevel(){

    return analogRead(PIN_SMOKE);
   

}



void init_tone(){
    pinMode(PIN_TONE,OUTPUT);  
    
}

void init_btn(){
    pinMode(PIN_BTN0,INPUT);  
}

void init_ff(){
    pinMode(PIN_FF,INPUT);  
}

void init_bls(){
    pinMode(PIN_BLS,INPUT);  
}

void init_led(){
    pinMode(1,OUTPUT);
    pinMode(2,OUTPUT);
    pinMode(3,OUTPUT);
    pinMode(4,OUTPUT);
}

void init_texi(){

   // texi_timer0=millis();
  
}

void init_display(){
   pinMode(PIN_LATCH,OUTPUT);
  pinMode(PIN_CLOCK,OUTPUT);
  pinMode(PIN_DATA,OUTPUT); //让三个脚都是输出状态  
}

void init_smoke(){
      pinMode(PIN_SMOKE, INPUT);
}



void setNumber(int n){
      disbuff[0]=n/1000;    //显示SUM千位值
    disbuff[1]=n%1000/100;  //显示SUM百位值
    disbuff[2]=n%100/10;    //显示SUM十位值
    disbuff[3]=n%10;        //显示SUM个位值
 }

void tone_on(){
    digitalWrite(PIN_TONE,LOW);//鍙戝０闊�
}

void tone_off(){
    digitalWrite(PIN_TONE,HIGH);
}

void tone_molody(int times){
  for(int i=0;i<times;i++){
// iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(PIN_TONE, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    tone_off();
    }
  }
}
