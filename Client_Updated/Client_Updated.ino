#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>
#include <stdlib.h>

#define RF95_FREQ 915.0
const char CLIENT_ID[] = "CL01";

RH_RF95 driver(8, 3);

void setup() {
  Serial.begin(115200);
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
  Serial.println("Client initialized.");
}

void loop() {
  //Send a heartbeat message every 5 seconds
  /*
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    lastSend = millis();
    char msg[64];
    snprintf(msg, sizeof(msg), "SRC:%s;TYPE:HEARTBEAT;MSG:Hello from %s", CLIENT_ID, CLIENT_ID);
    driver.send((uint8_t*)msg, strlen(msg));
    driver.waitPacketSent();
  }
  */
  //Check for Serial input from Raspberry Pi
  if (Serial.available()) {
    String piMessage = Serial.readStringUntil('\n');
    piMessage.trim();
    if (piMessage.length() > 0) {
      char sendMessage[RH_RF95_MAX_MESSAGE_LEN];
      snprintf(sendMessage, sizeof(sendMessage), "SRC:%s;MSG:%s", CLIENT_ID, piMessage.c_str());

      Serial.print("Sending Pi message via LoRa: ");
      Serial.println(sendMessage);

      driver.send((uint8_t*)sendMessage, strlen(sendMessage));
      driver.waitPacketSent();
    }
  }

  //Listen for messages from a Tower
  if (driver.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (driver.recv(buf, &len)) {
      buf[len] = 0; 

      //Extract EXC field if it is there
      char excID[10] = "";
      char *excPtr = strstr((char*)buf, "EXC:");
      if (excPtr) {
        excPtr += 4;
        int i = 0;
        while (excPtr[i] != ';' && excPtr[i] != '\0' && i < (int)sizeof(excID) - 1) {
          excID[i] = excPtr[i];
          i++;
        }
        excID[i] = '\0';
      }

      //Ignore messages that were originally sent by this client
      if (strcmp(excID, CLIENT_ID) == 0) {
        return;
      }

      Serial.print("Received LoRa message: ");
      Serial.println((char*)buf);

      //Forward the message to the Raspberry Pi via Serial
      Serial.println((char*)buf);

      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  delay(100);
}
