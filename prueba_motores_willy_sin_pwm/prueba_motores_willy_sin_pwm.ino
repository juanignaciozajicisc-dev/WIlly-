// Prueba de motores a habilitacion fija (sin PWM).
// Mantener retirados los jumpers ENA y ENB.
// GPIO14 -> ENA, GPIO32 -> ENB.
// Motor 1: OUT1/OUT2. Motor 2: OUT3/OUT4.
// Alimentar los motores por Vs/+12V y GND del L298N; unir GND con el ESP32.

constexpr uint8_t ENA_PIN = 14;
constexpr uint8_t IN1_PIN = 27;
constexpr uint8_t IN2_PIN = 26;
constexpr uint8_t ENB_PIN = 32;
constexpr uint8_t IN3_PIN = 25;
constexpr uint8_t IN4_PIN = 33;

constexpr uint32_t MOVE_TIME_MS = 1500;
constexpr uint32_t PAUSE_TIME_MS = 700;

void setMotor(int direction, uint8_t inA, uint8_t inB, uint8_t enablePin) {
  // Deshabilita el canal mientras cambia de direccion.
  digitalWrite(enablePin, LOW);

  if (direction > 0) {
    digitalWrite(inA, HIGH);
    digitalWrite(inB, LOW);
  } else if (direction < 0) {
    digitalWrite(inA, LOW);
    digitalWrite(inB, HIGH);
  } else {
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
  }

  // Sin PWM: HIGH habilita el canal a maxima potencia disponible.
  digitalWrite(enablePin, direction == 0 ? LOW : HIGH);
}

void setMotors(int motor1Direction, int motor2Direction) {
  setMotor(motor1Direction, IN1_PIN, IN2_PIN, ENA_PIN);
  setMotor(motor2Direction, IN3_PIN, IN4_PIN, ENB_PIN);
}

void stopMotors() {
  setMotors(0, 0);
}

void testStep(const char *description, int motor1Direction, int motor2Direction) {
  stopMotors();
  delay(PAUSE_TIME_MS);

  Serial.println(description);
  setMotors(motor1Direction, motor2Direction);
  Serial.printf("Motor 1=%d, Motor 2=%d (ENA=%d, ENB=%d)\n",
                motor1Direction, motor2Direction,
                digitalRead(ENA_PIN), digitalRead(ENB_PIN));
  delay(MOVE_TIME_MS);

  stopMotors();
  delay(PAUSE_TIME_MS);
}

void setup() {
  Serial.begin(115200);

  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  stopMotors();

  Serial.println("Prueba sin PWM: empieza en 2 segundos. Ruedas levantadas.");
  delay(2000);

  testStep("1) Motor 1 adelante; motor 2 detenido", 1, 0);
  testStep("2) Motor 2 atras; motor 1 detenido", 0, -1);
  testStep("3) Motor 1 atras; motor 2 detenido", -1, 0);
  testStep("4) Motor 2 adelante; motor 1 detenido", 0, 1);
  testStep("5) Motores 1 y 2 adelante", 1, 1);
  testStep("6) Motores 1 y 2 atras", -1, -1);

  stopMotors();
  Serial.println("Prueba terminada; motores detenidos.");
}

void loop() {
  // La secuencia se ejecuta una sola vez al arrancar o reiniciar.
  delay(1000);
}
