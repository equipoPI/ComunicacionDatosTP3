#include <IRremote.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
IRsend irsend;

int BT_Rx = 10;
int BT_Tx = 11;

SoftwareSerial BT(BT_Rx, BT_Tx);

#define LED_con 2
#define IR_emitter 9

byte registroR = 0;
byte registroG = 0;
byte registroB = 0;

int dato = 0;
int dato1 = 0;
int dato2 = 0;

byte flag = false;

void setup() {
  // put your setup code here, to run once:
  irsend.begin(IR_emitter);   //funcion de la libreria IRremote para defini el pin donde se conecta el emisor
  Serial.begin(9600);
  BT.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
 lectura();
 deteccion();

  envio();
}

void lectura() {
  if (BT.available() > 2) {
    dato1 = BT.read();
    dato2 = BT.read();
    dato = (dato2 * 256) + dato1;
  }
}

void guardado() {
  if (flag == true) {
    EEPROM.write(0, registroR);
    EEPROM.write(1, registroG);
    EEPROM.write(2, registroB);
    flag = false;
  }
}

void deteccion() {
  if (dato == 1) { //lee desde la eeprom y manda a prender
    registroR = EEPROM.read(0);
    registroG = EEPROM.read(1);
    registroB = EEPROM.read(2);
  }
  if (dato == 2) { //manda un 0 en todos los colores ya que es la señal de apagado
    registroR = 0;
    registroG = 0;
    registroB = 0;
  }
  if (dato == 3) { //llegada de la señal de aguradado en la eeprom
    dato = 0;
    flag = true;
    guardado();
  }

  if (dato >= 1000 && dato <= 1255) {
    registroR = map(dato, 1000, 1255, 0, 255);
    //Serial.println("rojo");
    //Serial.println(registroR);
  }
  if (dato >= 2000 && dato <= 2255) {
    registroG = map(dato, 2000, 2255, 0, 255);
    //Serial.println("verde");
    //Serial.println(registroG);
  }
  if (dato >= 3000 && dato <= 3255) {
    registroB = map(dato, 3000, 3255, 0, 255);
    //Serial.println("azul");
    //Serial.println(registroB);
  }
}

void envio() {
  // irsend.sendNEC(0x64, 0x64, 1);
  irsend.sendNEC(32, registroR, 1);

  // irsend.sendNEC(0x64, 0x64, 1);
  irsend.sendNEC(33, registroG, 1);

  // irsend.sendNEC(0x64, 0x64, 1);
  irsend.sendNEC(34, registroB, 1);
}
