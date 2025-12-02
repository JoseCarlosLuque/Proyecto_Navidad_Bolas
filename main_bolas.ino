// Pines driver BTS7960
const int RPWM = 9;   // PWM derecha
const int LPWM = 10;  // PWM izquierda

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

bool modoServicio = false;

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);

  pinMode(PIN_TOP, INPUT_PULLUP);

  pinMode(PIN_SUBIR, INPUT_PULLUP); // pulsado -> LOW
  pinMode(PIN_BAJAR, INPUT_PULLUP); // pulsado -> LOW

  stopMotor();
}

void loop() {

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
  driveDown(pwm_down);
  delay(t_bajar);
  stopMotor();
  delay(t_pausa);

  if (digitalRead(PIN_SUBIR) == LOW || digitalRead(PIN_BAJAR) == LOW) { modoServicio = true; return; }

}


// ================== FUNCIONES ==================

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

    // *** Si quieres volver al modo auto al soltar ambos botones ***
    // descomenta:
    // if (digitalRead(PIN_SUBIR)==HIGH && digitalRead(PIN_BAJAR)==HIGH) {
    //   modoServicio = false;
    //   return;
    // }

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

    if (digitalRead(PIN_TOP) == LOW) break;

    driveUp(pwm_up);
  }

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