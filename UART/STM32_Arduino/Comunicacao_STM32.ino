void setup() {
  Serial.begin(9600);     // Conecta ao PC
  Serial2.begin(9600);    // Conecta ao STM32
  Serial.println("Monitor disponivel");
}

void loop() {
  // STM32 → IDE Arduino
  if (Serial2.available()) {
    String msg = Serial2.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      Serial.print("Recebido do STM32: ");
      Serial.println(msg);
    }
  }

  // IDE Arduino → STM32
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      Serial.print("Mensagem enviada para o STM:");
      Serial.println(msg);
      Serial2.println(msg);
    }
  }
  delay(100);
}
