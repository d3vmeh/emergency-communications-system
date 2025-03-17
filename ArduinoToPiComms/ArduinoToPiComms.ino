void setup() {
  Serial.begin(115200);
  while (!Serial) { }  // Wait for Serial connection
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

  Serial.println("Hello Pi");  // Send message every second
  digitalWrite(LED_BUILTIN,HIGH);
  delay(1000);  // Wait to avoid spamming

  if (Serial.available() > 0) {
    String ack = Serial.readStringUntil('\n');  // Read full response from Pi
    ack.trim();
    digitalWrite(LED_BUILTIN,LOW);

    //Serial.print("Raspberry Pi says: ");
    //Serial.println(ack);
    if (ack.length() > 0 && ack[0] == 'R')
      Serial.println("Arduino Confirms Reception");
  }
}
