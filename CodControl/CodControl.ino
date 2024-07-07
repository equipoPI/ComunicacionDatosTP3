//Prebiamente configuramos con otro codigo el modulo bluetooth, usando comandos AT y las comunicacionnes seriales
//definimos nuevamente la contraseña del modulo como 1234, su velocidad de comunciacion en 9600 baud y su modo de trabajo como esclavo

#include <IRremote.h>          //libreria que usaremos para transmitir los datos por infrarojo
#include <EEPROM.h>            //libreria que usaremos para guardar un dato en la memoria eeprom
#include <SoftwareSerial.h>    //libreria que usaremos para transmitir y recibir los datos que llegan desde el modulo bluetooth
IRsend irsend;

int BT_Rx = 10;                //pines conectados al modulo bluetooth
int BT_Tx = 11;

SoftwareSerial BT(BT_Rx, BT_Tx); //definimos con la libreria SoftwareSerial la comunicacion arduino, siempre que se mencione BT.comandoserial
//se ejecutara el comando serialsolo que utilizando los pines definidos anterormente
//esto lo hacemos con la finalidad de no estar conectando y desconectando el modulo bluetooth cada vez que se carga un codigo
//nuevo y tambien no queda disponible el puerto serie para ver datos de control en la PC.

#define LED_con 2                //pin donde se conecta un len para visualizar el estado de conexion del modulo
#define IR_emitter 9             //pin donde esta conectado el emisor infrarojo
#define control_estado 3         //pin donde leeremos el estado del modulo bluetooth, este tiene un pin de estado que se encuentra en alto cuando esta conectado 
//algun dispositivo y esta en estado bajo cuando no hay ningun dispositivo conectado

byte registroR = 255;            //valores iniciales del los colores
byte registroG = 255;
byte registroB = 255;

//Variables de control y/o lectura, se usan para leer y convertir los valores que vienen del arduino con formato taxto y que luego nosotros
//convertimos a numerico para poder enviar por el emisor infrarojo
char valor;
String estado;

//Variables de control de estados variados
byte flag = false;
byte disp_estado = false;

//variables de control para el muestreo o ejecucion de funciones
byte g = 0;
byte x = 0;
byte t = 0;

void setup() {
  pinMode(LED_con, OUTPUT);         //defino el pin como salida
  pinMode(control_estado, INPUT);   //defino el pin como entrada

  irsend.begin(IR_emitter);   //funcion de la libreria IRremote para definir el pin donde se conecta el emisor
  Serial.begin(9600);         //seteo la velocidad de comunicacion serial con la PC en 9600
  BT.begin(9600);             //seteo la velocidad de comunicacion con el modulo BT en 9600
}

void loop() {
  arranque();                  //funcion que le la memoria eeprom

  control_conexion();          //funcion deteccion del pin de estado del modulo BT

  lectura();                   //funcion encargada de recibir variables tipo texto desde el modulo, a su vez tambien
  //transforma esos datos en variables numericas aptas para su trabajo

  impresionControl();        //funcion que manda datos por el serial de la PC como forma de control

  envio();                     //funcion que envia los datos por infrarojo

  enviarValoresRGB();          //envia datos al modulo BT para retroalimentacion a la aplicacion del celular
}

void impresionControl() {        //imprime los datos obtenidos para verificar que esten llegando los datos correctamente
  Serial.print("rojo");
  Serial.print(registroR);
  Serial.print(" verde");
  Serial.print(registroG);
  Serial.print("  azul");
  Serial.println(registroB);
}

void lectura() {
  if (BT.available()) { //Si el puerto serie (Bluetooth) está disponible
    valor = BT.read();  //Lee el dato entrante via Bluetooth
    Serial.println(valor);

    if (valor == 'R') { //Si el dato entrante es una B
      g = 1;                 //en funcion del valor de g se guarda en un registro diferente de color
      obtencionEntero();     //Llama la función que controla el valor a guradar el valor numerico de cada color
    }

    if (valor == 'G') {  //Si el dato entrante es una B
      g = 2;
      obtencionEntero();     //Llama la función que controla el valor a guradar el valor numerico de cada color
    }

    if (valor == 'B') {  //Si el dato entrante es una B
      g = 3;
      obtencionEntero();     //Llama la función que controla el valor a guradar el valor numerico de cada color
    }

    if (valor == 'S') {  //Si el dato entrante es una S
      flag = true;
      guardado();        //se llama a la funcion de guardar valores en la eeprom
      Serial.println("guardado");
    }

    if (valor == 'P') {  //Si el dato entrante es una P
      registroR = EEPROM.read(0);  //lee los valores guradados en la eeprom y prende el led con esos valores
      registroG = EEPROM.read(1);
      registroB = EEPROM.read(2);
    }

    if (valor == 'O') {  //Si el dato entrante es una O
      registroR = 0;     //los registros se ponen en 0 ya que es la señal de apagado
      registroG = 0;
      registroB = 0;
      Serial.println("Apagado");
    }

    if (valor == 'E') {  //Si el dato entrante es una E
      t = 0;             //pone t en 0, cuando se recibe una E indica que la concecion de la app android fud exitosa
      //al ponerse t en 0 la funcion de enviarValoresRGB(); se podra activar
    }
  }
}

void obtencionEntero() {
  delay(30);
  while (BT.available()) {
    char c = BT.read();   //Lee el dato entrante y lo almacena en una variable tipo char
    estado += c;     //Crea una cadena tipo String con los datos entrates
  }

  if (estado.length() > 0) { //Se verifica que la cadena tipo String tenga un largo mayor a cero

    if (g == 1) {                  //dependiendo del valor de g
      registroR = estado.toInt();  //Guarda en un registro el dato en forma de entero (int)
    }

    if (g == 2) {
      registroG = estado.toInt();  //Guarda en un registro el dato en forma de entero (int)
    }

    if (g == 3) {
      registroB = estado.toInt();  //Guarda en un registro el dato en forma de entero (int)
    }

    g = 0;            //vuelve la variable g a 0
    estado = "";      //Limpia la variable para poder leer posteriormente nuevos datos
  }
}

void arranque() { //funcion que lee los datos guardados en la eeprom un vez, los guarda en los registros correspondientes para luego mandarlos por el led IR 
  if (x == 0) {
    registroR = EEPROM.read(0);
    registroG = EEPROM.read(1);
    registroB = EEPROM.read(2);
    x = x + 1;
  }
}

void guardado() {                //la funcion se encarga de guardar los valores de las variables registros en en posiciones de la memoria eeprom, 
  if (flag == true) {            //cada posicion de la memoria tiene una longitud de 8 bits por lo que el espacio es suficiente para poner el valor maximo(255) 
    EEPROM.write(0, registroR);  //de cada registro. El arduino tiene una cantidad de 1024 posiciones de memoria
    EEPROM.write(1, registroG);
    EEPROM.write(2, registroB);
    flag = false;
  }
}

void envio() {                       //funcion de envio de los colores al decodificador por medio de infrarojoa, a cada color le asignamos
  // irsend.sendNEC(0x64, 0x64, 1);  //una direccion entonces dependiendo del color a transmitir le llegaria una o troa diresccion para identificarlo
  irsend.sendNEC(32, registroR, 1);

  // irsend.sendNEC(0x64, 0x64, 1);
  irsend.sendNEC(33, registroG, 1);

  // irsend.sendNEC(0x64, 0x64, 1);
  irsend.sendNEC(34, registroB, 1);
}

void enviarValoresRGB() {
  // Enviar los valores RGB actuales a la aplicación a través de Bluetooth
  // Se ejecuta cada vez que se recibe el caracter que indica una nueva conexion ('E')
  if ( t == 0) {                     //La transmicion se realiza de esta forma ya que la aplicacion android lo recibira todo como texto y utilizara 
    BT.print(registroR);             // | para delimitar cada valor y asignare a cada uno una posicion en una lista que podra ser interpretada como numero
    BT.print("|");                   //eso no ayudara para actualizar el canvas que muestra los colores y mostrar la conbinacion de valores que representa ese color
    BT.print(registroG);
    BT.print("|");
    BT.print(registroB);
    BT.print("\n"); // Fin de línea. Importante.
    t = t + 1;
  }
  /*
    String R = (String) registroR;
    String G = (String) registroG;
    String B = (String) registroB;

    total = R + "," + G + "," + B;
    BT.println(total);

    delay(300);
  */
  //delay(2000);
  //Serial.println("Valores RGB enviados a la aplicación");
}

void control_conexion() {                     //Esta funcion se encarga de controlar el pin de estado del modulo el cual indica si hay una conexion establecida o no
  disp_estado = digitalRead(control_estado);  //esto lo hacemo por que el indicador led que viene con el modulo puede genrar confuciones ya que hace las siguientes acciones:
  if (disp_estado == 1) {                     //                 Conexion establecida==> parpadeo lento
    digitalWrite(LED_con, HIGH);              //                 Sin conexion==> parpadeo rapido
  }                                           //                 Modo de configuracion AT1 o AT2==> parpadeo mas lento que el de conexion establecida
  if (disp_estado == 0) {                     //para no generar conflictos el arduino chequea el estado del modulo y si hay conexion prende un led mientras la conexion este establecida y si pasa
    digitalWrite(LED_con, LOW);               //lo contrario el led se apaga o no se prende
  }
  Serial.print("Estado modulo");
  Serial.println(disp_estado);
}
