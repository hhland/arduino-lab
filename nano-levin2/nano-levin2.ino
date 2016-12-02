#define PT_USE_TIMER
#define PT_USE_SEM

#include "pitches.h"
#include <DHT.h>
 #include "pt.h"
 #include "pt-sem.h"

#define DHTTYPE         DHT22

static struct pt thread1,thread2; 
static struct pt_sem sem_LED;

const int PIN_TONE=3;
const int PIN_LATCH = 4;
const int PIN_CLOCK =7;
const int PIN_DATA = 8; //这里定义了那三个脚
const int PIN_BLS=9;
const int PIN_LM35=A4;
const int PIN_SMOKE=A5;
const int PIN_DHT=6;
const int PIN_PM25=6;

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
int Flag_up = 1;
int Flag_up1 = 1;

int lcd_delay=0;

int mode=3;

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

DHT dht(PIN_DHT, DHTTYPE);

void setup() {

  
  // put your setup code here, to run once:
   Serial.begin(9600);
   //dht.begin();
   //init_pt();
    init_tone();
    init_pm25();
    //tone_molody(1);
    tone_on();
    delay(1000);
    tone_off();
    init_bls();
    //init_temp();
    //init_smoke();
    init_display();
    
    setNumber(0);
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
   onButtonPress();
    if(mode==1){
     //bls
       display();
       onBLSRead(digitalRead(PIN_BLS));
               
    }else if(mode==2){
       tone_molody(1);
       delay(500);
    }else  if(mode==3){

       if(lcd_delay>0){
      display();
   }else{
      duration = pulseIn(PIN_PM25, LOW);
       lowpulseoccupancy = lowpulseoccupancy+duration;
   }
      
       //tone_molody(1);
       //l=digitalRead(PIN_BLS);
         onBLSRead(digitalRead(PIN_BLS));
        //duration = pulseIn(PIN_PM25, LOW);
       //lowpulseoccupancy = lowpulseoccupancy+duration;
       if ((millis()-starttime) > sampletime_ms)
      {
        float r = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
        
        //Serial.print(lowpulseoccupancy);
        onPM25Read(r);
        lowpulseoccupancy = 0;
        starttime = millis();
      }

       if(lcd_delay>0){
        lcd_delay=lcd_delay-1;  
    }else {
        //lcd_delay=0;
        setNumber(0);
        //display();
    } 
      
    }
   
   //thread2_entry(&thread2);
  // display();
   
    //Serial.print(lcd_delay);
}

static int thread1_entry(struct pt *pt)
{
    PT_BEGIN(pt);
    while (1)
    {
    PT_SEM_WAIT(pt,&sem_LED); //LED有在用吗？
    display();
    
    //PT_TIMER_DELAY(pt,1000);//留一秒
    PT_SEM_SIGNAL(pt,&sem_LED);//用完了。
    PT_YIELD(pt); //看看别人要用么？
    }
    PT_END(pt);
}

static int thread2_entry(struct pt *pt)
{
    PT_BEGIN(pt);
    while (1)
    {
    PT_SEM_WAIT(pt,&sem_LED); //LED有在用吗？
    
     //l=readSmokeLevel();
    //onSmokeLevelRead(l);
   
    //duration = pulseIn(PIN_PM25, LOW);
    lowpulseoccupancy = lowpulseoccupancy+duration;
    //PT_TIMER_DELAY(pt,1000);//留一秒
    PT_SEM_SIGNAL(pt,&sem_LED);//用完了。
    PT_YIELD(pt); //看看别人要用么？
    }
    PT_END(pt);
}

void onButtonPress(){
     if(digitalRead(A1)==LOW){
         mode=1; 
         SUM_BLS=0;
         setNumber(SUM_BLS);
         delay(500); 
     }
     if(digitalRead(A2)==LOW){
        mode=2;
         delay(500);
     }
     if(digitalRead(A3)==LOW){
          mode=3; 
         
         delay(500);
     } 
     
  
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
    tone_on();
    delay(800);
    tone_off(); 
    lcd_delay=6000;
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

int readSmokeLevel(){

    return analogRead(PIN_SMOKE);
   

}

void init_pt(){
   PT_SEM_INIT(&sem_LED,1);   
   //初始化任务记录变量
    PT_INIT(&thread1);
    PT_INIT(&thread2);
}

void init_tone(){
    pinMode(PIN_TONE,OUTPUT);  
    
}

void init_bls(){
    pinMode(PIN_BLS,INPUT);  
}

void init_temp(){
    
}



void init_display(){
   pinMode(PIN_LATCH,OUTPUT);
  pinMode(PIN_CLOCK,OUTPUT);
  pinMode(PIN_DATA,OUTPUT); //让三个脚都是输出状态  
}

void init_smoke(){
      pinMode(PIN_SMOKE, INPUT);
}

void init_pm25(){
      pinMode(PIN_PM25,INPUT);
      starttime = millis();
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
