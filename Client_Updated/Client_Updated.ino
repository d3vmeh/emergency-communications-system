#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>
#include <stdlib.h>

#define RF95_FREQ 915.0

// Unique Client ID – change this per device (e.g. CL01, CL02, etc.)
const char CLIENT_ID[] = "CL01";

// RH_RF95 configuration for Adafruit Feather M0 with RFM95 (CS on pin 8, IRQ on pin 3)
RH_RF95 driver(8, 3);

void setup() {
  Serial.begin(115200);  // Ensure the Pi reads at 115200 baud.
  while (!Serial) { } // Wait for Serial connection.

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

  Serial.println("Client initialized.");
}

void loop() {
  static unsigned long lastSend = 0;
  
  //Send a heartbeat message every 5 seconds.
  /*
  if (millis() - lastSend > 5000) {
    lastSend = millis();
    char msg[64];
    snprintf(msg, sizeof(msg), "SRC:%s;TYPE:HEARTBEAT;MSG:Hello from %s", CLIENT_ID, CLIENT_ID);
    
    driver.send((uint8_t*)msg, strlen(msg));
    driver.waitPacketSent();
  }
  */
  
  //Listen for messages from the Raspberry Pi via Serial
  if (Serial.available()) {
    String piMessage = Serial.readStringUntil('\n');
    piMessage.trim();
    if (piMessage.length() > 0) {
      Serial.print("Sending Pi message via LoRa: ");
      Serial.println(piMessage);

      driver.send((uint8_t*)piMessage.c_str(), piMessage.length());
      driver.waitPacketSent();
    }
  }

  //Listen for messages from the Tower
  if (driver.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (driver.recv(buf, &len)) {
      buf[len] = 0; 

      //Check for an exclusion field ("EXC:") in the received message
      char *excPtr = strstr((char*)buf, "EXC:");
      if (excPtr != NULL) {
        excPtr += 4;  
        char excID[10];
        int i = 0;
        while (excPtr[i] != ';' && excPtr[i] != '\0' && i < sizeof(excID) - 1) {
          excID[i] = excPtr[i];
          i++;
        }
        excID[i] = '\0';
        if (strcmp(excID, CLIENT_ID) == 0) {
          return;
        }
      }
      
      Serial.print("Received LoRa message: ");
      Serial.println((char*)buf);

      //Forward message to the Raspberry Pi
      Serial.println((char*)buf);

      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  delay(100);
}
