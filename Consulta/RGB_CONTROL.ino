#include <SoftwareSerial.h>
int BT_Rx = 10;
int BT_Tx = 11;

SoftwareSerial BT(BT_Rx,BT_Tx);

int LED_R = 3;  
int LED_G = 5; 
int LED_B= 6;// Needs to be a pin supporting PWM

int rojo;
int verde;
int azul;

void setup()

{

  Serial.begin(9600);
  BT.begin(9600);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

}



void loop()

{
  if (BT.available()>2){
  int dato1 = BT.read();
  int dato2 = BT.read();
  int dato = (dato2*256)+dato1;
  //Serial.println(dato);

  if (dato >=1000 && dato <= 1255) {
    rojo = map(dato,1000,1255,0,255);
    //Serial.println("rojo");
    //Serial.println(rojo);
    analogWrite(LED_R,rojo);
  }
   if (dato >=2000 && dato <= 2255) {
    verde = map(dato,2000,2255,0,255);
    //Serial.println("verde");
    //Serial.println(verde);
    analogWrite(LED_G,verde);
  }
   if (dato >=3000 && dato <= 3255) {
    azul = map(dato,3000,3255,0,255);
    //Serial.println("azul");
    //Serial.println(azul);
    analogWrite(LED_B,azul);
  }
  }

  
  }
