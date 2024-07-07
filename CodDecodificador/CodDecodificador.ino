#include <IRremote.h> //incluimos libreria para el control de las señales de infrarojo en la version 3.3.0

// Definir pines para el LED RGB y llegada del infrarojo
#define PIN_LED_RED 8
#define PIN_LED_GREEN 7
#define PIN_LED_BLUE 6
#define IRpin 2

#define CODIGO_RED 32         //codigo que identifica el color rojo
#define CODIGO_GREEN 33       //codigo que identifica el color verde
#define CODIGO_BLUE 34        //codigo que identifica el color azul

int state = 0;
int control = 0;

int intensidadR = 0;
int intensidadG = 0;
int intensidadB = 0;

void setup() {
  // Inicializar el receptor infrarrojo
  IrReceiver.begin(IRpin); //definimos como entrada de señal infraroja utilizando la sentencia de la libreria

  //iniciamos el puerto serie para control de la modificacion de variables y llegada de datos de señal infraroja
  Serial.begin(9600);

  //Configurar los pines del LED RGB como salidas
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
}

void loop() {
  if (IrReceiver.decode()) {
    state = IrReceiver.decodedIRData.address; //esta sentencia decodifica la señal recibida y la guarda en la variable state
    control = IrReceiver.decodedIRData.command; //esta sentencia decodifica el tamaño de la señal mandada y la guarda en control

    if (state == CODIGO_RED) {        //funcion para guardar la señal recibida desde el emisor para prender el controla en el valor guardado
      intensidadR = control;         //es para el color rojo
    }

    if (state == CODIGO_GREEN) {               //se repite lo mismo para el color verde y azul en la linea anterior
      intensidadG = control;
    }

    if (state == CODIGO_BLUE) {
      intensidadB = control;
    }
    IrReceiver.resume();
  }

Serial.print(intensidadR);
Serial.print("  ");
Serial.print(intensidadG);
Serial.print("  ");
Serial.println(intensidadB);
  
  analogWrite(PIN_LED_RED, intensidadR);
  analogWrite(PIN_LED_GREEN, intensidadG);
  analogWrite(PIN_LED_BLUE, intensidadB);
}
