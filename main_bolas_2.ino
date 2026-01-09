#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Pines driver BTS7960
const int RPWM = 9;   // PWM derecha
const int LPWM = 10;  // PWM izquierda

// Pines modulo radio frecuencia; 
RF24 radio(8, 5);
const byte direccion[6] = "DISCO";


// Final de carrera superior
const int PIN_TOP = 4;  

// Botones de servicio
const int PIN_SUBIR = 6;   
const int PIN_BAJAR = 7;

// Parámetros del movimiento automático
const int pwm_up = 240; // Valor original 180 -> El máximo es 255
const int pwm_down = 240; // Valor original 180 -> El máximo es 255


const unsigned long t_bajar = 12000;  // Tiempo expresado en milisegundos dividir por 1000 
const unsigned long t_pausa = 1000;   // milisegundos.


// Dos variables de estado, para poder modificarlo cuando se necesite.
bool modoServicio = false;
bool motorEnMarcha = false; // Por seguridad, arrancan apagados

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);

  pinMode(PIN_TOP, INPUT_PULLUP);

  pinMode(PIN_SUBIR, INPUT_PULLUP); // pulsado -> LOW
  pinMode(PIN_BAJAR, INPUT_PULLUP); // pulsado -> LOW

  radio.begin(); // Iniciazmos la radio
  radio.openReadingPipe(1, direccion); // Establecemos el canal
  radio.setPALevel(RF24_PA_LOW); // Potencia de la señal
  radio.setDataRate(RF24_250KBPS); // Velocidad 
  radio.setChannel(115); // Canal
  radio.startListening();

  stopMotor();
} 

void loop() {

  if (radio.available()) {
    byte comandoRecibido;
    radio.read(&comandoRecibido, sizeof(comandoRecibido));
    
    if (comandoRecibido == 0xA1) motorEnMarcha = true;
    if (comandoRecibido == 0xB2) motorEnMarcha = false;
  }

  // Leemos el estado en cada vuelta, y paramos o efectuamos un ciclo de movimiento.
  if (motorEnMarcha) {
    ejecutarCicloMovimiento();
  } else {
    stopMotor();
  }
}


// ================== FUNCIONES ===================

void ejecutarCicloMovimiento() {
  // ----------- DETECCIÓN DE BOTONES MANUALES ----------------
  if (digitalRead(PIN_SUBIR) == LOW || digitalRead(PIN_BAJAR) == LOW) {
    modoServicio = true;
  }

  // ----------- MODO SERVICIO (manual) -----------------------
  if (modoServicio) {
    controlManual();
    return;   // NO volver al ciclo automático
  }

  // ------------------- MODO AUTOMÁTICO ----------------------

  // 1. Garantizar posición arriba
  subirHastaFinal();
  delay(t_pausa);

  if (digitalRead(PIN_SUBIR) == LOW || digitalRead(PIN_BAJAR) == LOW) { modoServicio = true; return; }

  // 2. Bajar por tiempo
  bajarPorTiempo();
  stopMotor();
  delay(t_pausa);

  if (digitalRead(PIN_SUBIR) == LOW || digitalRead(PIN_BAJAR) == LOW) { modoServicio = true; return; }
}

void controlManual() {
  stopMotor();

  // Control manual continuo
  while (true) {

    // SUBIR MANUAL
    if (digitalRead(PIN_SUBIR) == LOW) {

      // Seguridad: no permitir subir si el final de carrera está activado
      if (digitalRead(PIN_TOP) == HIGH) {
        driveUp(pwm_up);
      } else {
        stopMotor();
      }
    }

    // BAJAR MANUAL
    else if (digitalRead(PIN_BAJAR) == LOW) {
      driveDown(pwm_down);
    }

    // Ningún botón pulsado → parar
    else {
      stopMotor();
    }

    // Comentar para tener el código normal

    // Si se aprietan los dos se vuelve al modo normal.
    if (digitalRead(PIN_SUBIR)==LOW && digitalRead(PIN_BAJAR)==LOW) {
      modoServicio = false;
      return;
    }

    // Pequeño delay anti rebotes
    delay(20);
  }
}

void subirHastaFinal() {
  unsigned long maxTime = 120000;
  unsigned long start = millis();

  while (millis() - start < maxTime) {
    if (digitalRead(PIN_SUBIR) == LOW || digitalRead(PIN_BAJAR) == LOW) {
      modoServicio = true;
      stopMotor();
      return;
    }

    if (radio.available()) {
      byte comandoRecibido;
      radio.read(&comandoRecibido, sizeof(comandoRecibido));
      
      if (comandoRecibido == 0xB2) { // Si el mando dice PAUSA
        motorEnMarcha = false;
        stopMotor();
        return; // SALIDA DE EMERGENCIA: Rompe el método y vuelve al loop()
      }
    }

    if (digitalRead(PIN_TOP) == LOW) break;

    driveUp(pwm_up);
  }

  stopMotor();
}

void bajarPorTiempo() {
  unsigned long start = millis();

  // El bucle corre mientras no se agote el tiempo t_bajar
  while (millis() - start < t_bajar) {
    
    // 1. CONTROL MANUAL (Modo Servicio)
    if (digitalRead(PIN_SUBIR) == LOW || digitalRead(PIN_BAJAR) == LOW) {
      modoServicio = true;
      stopMotor();
      return; // Sale del método inmediatamente
    }

    // 2. CONTROL POR RADIO (Mando a distancia)
    if (radio.available()) {
      byte comandoRecibido;
      radio.read(&comandoRecibido, sizeof(comandoRecibido));
      
      if (comandoRecibido == 0xB2) { // Código de PAUSA
        motorEnMarcha = false;
        stopMotor();
        return; // Salida de emergencia al loop()
      }
    }

    // 3. ACCIÓN DE MOVIMIENTO
    // Si no ha pasado nada de lo anterior, el motor baja
    driveDown(pwm_down); 
  }

  // Al finalizar el tiempo, apagamos el motor
  stopMotor();
}

void driveUp(int pwm) {
  analogWrite(RPWM, pwm);
  analogWrite(LPWM, 0);
}

void driveDown(int pwm) {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, pwm);
}

void stopMotor() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
}