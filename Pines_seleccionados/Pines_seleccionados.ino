#include <SoftwareSerial.h>

int Tx = 7;
int Rx = 8;
SoftwareSerial BT(Tx, Rx);

char letra;
int LED  =  13;

void setup()
{
  pinMode(LED, OUTPUT);
  BT.begin(9600);
}

void loop()
{

  if (BT.available())
    letra = BT.read();
  if (letra == 'A')
  {
    digitalWrite(LED, 1);
  }
  if (letra == 'B')
  {
    digitalWrite(LED, 0);
  }

}
