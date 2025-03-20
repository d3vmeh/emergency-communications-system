#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>
#include <stdlib.h>
#define RF95_FREQ 915.0

const char TOWER_ID[] = "TW01";
RH_RF95 driver(8, 3);

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  if (!driver.init()) {
    Serial.println("LoRa init failed!");
    while (1);
  }

  if (!driver.setFrequency(RF95_FREQ)) {
    Serial.println("LoRa setFrequency failed!");
    while (1);
  }

  driver.setTxPower(15, false);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Tower initialized.");
}

void loop() {
  //Check for incoming LoRa messages
  if (driver.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (driver.recv(buf, &len)) {
      buf[len] = 0; 
      Serial.print("Received LoRa message: ");
      Serial.println((char*)buf);

      char forwardMessage[RH_RF95_MAX_MESSAGE_LEN];

      // Extract sender's ID (SRC)
      char senderID[10] = "";
      char *srcPtr = strstr((char*)buf, "SRC:");
      if (srcPtr) {
        srcPtr += 4;
        int i = 0;
        while (srcPtr[i] != ';' && srcPtr[i] != '\0' && i < (int)sizeof(senderID) - 1) {
          senderID[i] = srcPtr[i];
          i++;
        }
        senderID[i] = '\0';
      }

      //Prevent relaying if the message was already from the Tower
      if (strcmp(senderID, TOWER_ID) == 0) {
        return;
      }

      //Append EXC field to prevent the sender from receiving its own message
      snprintf(forwardMessage, sizeof(forwardMessage), "SRC:%s;MSG:%s;EXC:%s", TOWER_ID, (char*)buf, senderID);

      Serial.print("Broadcasting message: ");
      Serial.println(forwardMessage);

      driver.send((uint8_t*)forwardMessage, strlen(forwardMessage));
      driver.waitPacketSent();

      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  //Serial input for manual messages from the Tower
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
