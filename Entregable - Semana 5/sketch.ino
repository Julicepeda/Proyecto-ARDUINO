/*
 * POLITECNICO GRANCOLOMBIANO - IoT / Entrega 2
 * Arduino UNO R3: LED verde D2 y LED rojo D3.
 * Comandos de la orientacion: V, R, A, 0, X.
 * Serial UART a 9600 baudios; sin delay().
 * Las salidas se monitorean cada 500 ms con millis().
 *
 * USAR_HC05 = 0: Wokwi; comando por monitor serial.
 * USAR_HC05 = 1: HC-05 virtual / fisico (UART
 *   RX del Arduino D10, TX del Arduino D11).
 */
#define USAR_HC05 0

#if USAR_HC05
#include <SoftwareSerial.h>
SoftwareSerial hc05(10, 11); // RX, TX Arduino
#endif

const byte LED_VERDE = 2;
const byte LED_ROJO = 3;
const unsigned long INTERVALO_MS = 500;
unsigned long ultimaMuestra = 0;

// Buffer independiente por cada puerto serial.
char bufferUSB[8];
byte indiceUSB = 0;
bool desbordadoUSB = false;
#if USAR_HC05
char bufferBT[8];
byte indiceBT = 0;
bool desbordadoBT = false;
#endif

// CSV: conserva el tiempo en milisegundos y las
// salidas observadas de ambos LED.
void registrar(const char* evento, const char* origen,
              char comando, const char* resultado) {
  Serial.print(evento);
  Serial.print(',');
  Serial.print(millis());
  Serial.print(',');
  Serial.print(origen);
  Serial.print(',');
  Serial.print(comando);
  Serial.print(',');
  Serial.print(digitalRead(LED_VERDE));
  Serial.print(',');
  Serial.print(digitalRead(LED_ROJO));
  Serial.print(',');
  Serial.println(resultado);
}

// V: solo verde; R: solo rojo; A: ambos;
// 0: ninguno; X y otros: error, sin cambios.
void ejecutar(char comando, bool desdeBT) {
  bool valido = true;
  switch (comando) {
    case 'V':
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_ROJO, LOW);
      break;
    case 'R':
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_ROJO, HIGH);
      break;
    case 'A':
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_ROJO, HIGH);
      break;
    case '0':
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_ROJO, LOW);
      break;
    default:
      valido = false;
      break;
  }
  registrar("CMD", desdeBT ? "HC05" : "USB",
           comando, valido ? "OK" : "ERROR");
#if USAR_HC05
  if (desdeBT) {
    hc05.print(valido ? "OK" : "ERROR");
    hc05.print(" V=");
    hc05.print(digitalRead(LED_VERDE));
    hc05.print(" R=");
    hc05.println(digitalRead(LED_ROJO));
  }
#else
  (void)desdeBT;
#endif
}

// Lee la linea sin pausar el microcontrolador.
// Se acepta LF o CRLF, y un solo comando/letra.
void recibir(Stream& puerto, char* buffer,
             byte& indice, bool& desbordado,
             bool desdeBT) {
  while (puerto.available() > 0) {
    char c = (char)puerto.read();
    if (c == '\r') continue;
    if (c == '\n') {
      if (desbordado || indice != 1) {
        registrar("CMD", desdeBT ? "HC05" : "USB",
                 '?', "ERROR_FORMATO");
      } else {
        ejecutar(buffer[0], desdeBT);
      }
      indice = 0;
      desbordado = false;
    } else if (!desbordado) {
      if (indice < 7) buffer[indice++] = c;
      else desbordado = true;
    }
  }
}

void setup() {
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, LOW);
  Serial.begin(9600);
#if USAR_HC05
  hc05.begin(9600);
  hc05.println("HC05 LISTO: V R A 0 X");
#endif
  Serial.println(
    "evento,t_ms,origen,comando,verde,rojo,resultado");
  registrar("SYS", "ARDUINO", '-', "INICIO");
}

void loop() {
  recibir(Serial, bufferUSB, indiceUSB,
         desbordadoUSB, false);
#if USAR_HC05
  recibir(hc05, bufferBT, indiceBT,
         desbordadoBT, true);
#endif
  unsigned long ahora = millis();
  if (ahora - ultimaMuestra >= INTERVALO_MS) {
    ultimaMuestra = ahora;
    registrar("MUESTRA", "ARDUINO", '-', "OK");
  }
}
