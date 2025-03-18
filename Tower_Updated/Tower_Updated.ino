#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>
#include <stdlib.h>

#define RF95_FREQ 915.0

const char TOWER_ID[] = "TW01";

RH_RF95 driver(8, 3);

void setup() {
  Serial.begin(9600);
  while (!Serial) { } //Wait for Serial monitor

  if (!driver.init()) {
    Serial.println("LoRa init failed!");
    while (1);
  }

  if (!driver.setFrequency(RF95_FREQ)) {
    Serial.println("LoRa setFrequency failed!");
    while (1);
  }

  Serial.print("LoRa Frequency set to: ");
  Serial.println(RF95_FREQ);
  driver.setTxPower(15, false);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Tower initialized. Type a message and press Enter to send:");
}

void loop() {
  //Check if a LoRa message is available from any Client
  if (driver.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (driver.recv(buf, &len)) {
      buf[len] = 0;

      Serial.print("Received LoRa message: ");
      Serial.println((char*)buf);

      char forwardMessage[RH_RF95_MAX_MESSAGE_LEN];
      snprintf(forwardMessage, sizeof(forwardMessage), "SRC:%s;ORIG:%s", TOWER_ID, (char*)buf);

      Serial.print("Broadcasting message: ");
      Serial.println(forwardMessage);

      driver.send((uint8_t*)forwardMessage, strlen(forwardMessage));
      driver.waitPacketSent();

      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }


  if (Serial.available()) {
    String userMessage = Serial.readStringUntil('\n'); 
    userMessage.trim(); 

    if (userMessage.length() > 0) {
      char sendMessage[RH_RF95_MAX_MESSAGE_LEN];
      snprintf(sendMessage, sizeof(sendMessage), "SRC:%s;MSG:%s", TOWER_ID, userMessage.c_str());

      Serial.print("Sending manual message: ");
      Serial.println(sendMessage);

      driver.send((uint8_t*)sendMessage, strlen(sendMessage));
      driver.waitPacketSent();

      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  delay(100);
}
