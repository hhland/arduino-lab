
#include <IRremote.h>
#define TIMER_PWM_PIN  9 


 
const int PIN_IR_RECV = 2;


const int IR_FREQ=38;

const unsigned int IRCODE_AC_POWER[243]= {8950,4300,650,1550,650,1550,650,450,700,400,700,400,700,400,700,400,700,1500,700,400,700,1550,650,1550,650,450,650,450,650,450,650,400,700,400,700,400,700,450,650,1550,700,400,700,400,700,1500,700,1500,700,400,700,1500,700,
   1550,650,450,650,450,650,450,650,1550,650,450,650,1550,650,450,700,400,700,400,700,400,700,400,700,400,700
    }; 
 
IRrecv irrecv(PIN_IR_RECV);
IRsend irsend;



decode_results results;
 unsigned int buf[243];
 
void setup()
{
  Serial.begin(9600);
  init_ir();
  init_buttons();
  
}
 

 
 
void loop() {
  if (irrecv.decode(&results)) {
    onIRRecv(&results);
    irrecv.resume(); 
  }

  int val=digitalRead(A1);
   if(val==LOW){
      onA1Press();
   }
   val=digitalRead(A2);
   if(val==LOW){
      onA2Press();
   }

   val=digitalRead(A3);
   if(val==LOW){
      onA3Press();
   }
}

void onA1Press(){
      Serial.print("A1");
      ir_send_ac_power();
      delay(3000);
}

void onA2Press(){
  
}

void onA3Press(){
  
}

void onIRRecv(decode_results *results) {
  int count = results->rawlen;
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");
 
  for (int i = 0; i < count; i++) {
    if ((i % 2) == 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    } 
    else {
      Serial.print(-(int)results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(",");
  }
  Serial.println("");
}

void ir_send_ac_power(){
    irsend.sendRaw(IRCODE_AC_POWER,243,IR_FREQ);  
}

void init_buttons(){

   pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  
}


void init_ir(){
    irrecv.enableIRIn(); 
    pinMode(TIMER_PWM_PIN,OUTPUT);
}
