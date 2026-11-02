// Prueba independiente de los dos motores de Willy con ESP32 + L298N.
// Mantener retirados los jumpers ENA y ENB.
// GPIO14 -> ENA, GPIO32 -> ENB; conectar GND del ESP32 con GND del L298N.

constexpr uint8_t ENA_PIN = 14;
constexpr uint8_t IN1_PIN = 27;
constexpr uint8_t IN2_PIN = 26;
constexpr uint8_t ENB_PIN = 32;
constexpr uint8_t IN3_PIN = 25;
constexpr uint8_t IN4_PIN = 33;

constexpr uint32_t PWM_FREQUENCY = 5000;
constexpr uint8_t PWM_RESOLUTION = 8;
constexpr int MOTOR_SPEED = 220;
constexpr uint32_t MOVE_TIME_MS = 1500;
constexpr uint32_t PAUSE_TIME_MS = 700;

void setMotor(int speed, uint8_t inA, uint8_t inB, uint8_t enablePin) {
  speed = constrain(speed, -255, 255);

  if (speed > 0) {
    digitalWrite(inA, HIGH);
    digitalWrite(inB, LOW);
  } else if (speed < 0) {
    digitalWrite(inA, LOW);
    digitalWrite(inB, HIGH);
  } else {
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
  }

  ledcWrite(enablePin, abs(speed));
}

void setMotors(int motor1Speed, int motor2Speed) {
  setMotor(motor1Speed, IN1_PIN, IN2_PIN, ENA_PIN);
  setMotor(motor2Speed, IN3_PIN, IN4_PIN, ENB_PIN);
}

void stopMotors() {
  setMotors(0, 0);
}

void testStep(const char *description, int motor1Speed, int motor2Speed) {
  stopMotors();
  delay(PAUSE_TIME_MS);

  Serial.println(description);
  Serial.printf("Motor 1=%d, Motor 2=%d\n", motor1Speed, motor2Speed);
  setMotors(motor1Speed, motor2Speed);
  delay(MOVE_TIME_MS);

  stopMotors();
  delay(PAUSE_TIME_MS);
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);

  // Deja las habilitaciones bajas mientras se prepara el PWM.
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  digitalWrite(ENA_PIN, LOW);
  digitalWrite(ENB_PIN, LOW);

  const bool motor1PwmReady = ledcAttach(ENA_PIN, PWM_FREQUENCY, PWM_RESOLUTION);
  const bool motor2PwmReady = ledcAttach(ENB_PIN, PWM_FREQUENCY, PWM_RESOLUTION);

  if (!motor1PwmReady || !motor2PwmReady) {
    Serial.printf("Error al configurar PWM: motor 1=%s, motor 2=%s\n",
                  motor1PwmReady ? "OK" : "ERROR",
                  motor2PwmReady ? "OK" : "ERROR");
    if (motor1PwmReady) ledcWrite(ENA_PIN, 0);
    else digitalWrite(ENA_PIN, LOW);
    if (motor2PwmReady) ledcWrite(ENB_PIN, 0);
    else digitalWrite(ENB_PIN, LOW);
    while (true) delay(1000);
  }

  stopMotors();
  Serial.println("Inicio de la prueba en 2 segundos. Manten las ruedas levantadas.");
  delay(2000);

  testStep("1) Motor 1 adelante; motor 2 detenido", MOTOR_SPEED, 0);
  testStep("2) Motor 2 atras; motor 1 detenido", 0, -MOTOR_SPEED);
  testStep("3) Motor 1 atras; motor 2 detenido", -MOTOR_SPEED, 0);
  testStep("4) Motor 2 adelante; motor 1 detenido", 0, MOTOR_SPEED);
  testStep("5) Motores 1 y 2 adelante", MOTOR_SPEED, MOTOR_SPEED);
  testStep("6) Motores 1 y 2 atras", -MOTOR_SPEED, -MOTOR_SPEED);

  stopMotors();
  Serial.println("Prueba terminada; motores detenidos.");
}

void loop() {
  // La secuencia se ejecuta una sola vez al iniciar o reiniciar el ESP32.
  delay(1000);
}
