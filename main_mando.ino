#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(8, 5); // CE, CSN
const byte direccion[6] = "DISCO";

const int BTN_PLAY = 6;
const int BTN_PAUSE = 7;

void setup() {
  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_PAUSE, INPUT_PULLUP);

  radio.begin();
  radio.openWritingPipe(direccion);
  radio.setPALevel(RF24_PA_LOW);    // Potencia máxima para el mando
  radio.setDataRate(RF24_250KBPS); // Misma velocidad que el receptor
  radio.setChannel(115);           // Mismo canal que el receptor
  radio.stopListening();           // Modo emisor
}

void loop() {
  if (digitalRead(BTN_PLAY) == LOW) {
    enviarComando(0xA1); // Mandamos código de PLAY
    delay(200);          // Evitar rebotes del botón
  }

  if (digitalRead(BTN_PAUSE) == LOW) {
    enviarComando(0xB2); // Mandamos código de PAUSE
    delay(200);
  }
}

void enviarComando(byte comando) {
  // Enviamos el comando 15 veces muy rápido. 
  // Esto garantiza que si un paquete se pierde por el ruido, el siguiente llegue.
  for(int i = 0; i < 15; i++) {
    radio.write(&comando, sizeof(comando), true); // El 'true' activa multicast (no espera ACK)
  }
}