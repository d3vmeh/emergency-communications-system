#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>
#include <stdlib.h>

#define RF95_FREQ 915.0

// Unique Tower ID
const char TOWER_ID[] = "TW01";

RH_RF95 driver(8, 3);

void setup() {
  Serial.begin(9600);
  while (!Serial) { }

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
      buf[len] = 0; //Null-terminate the received message

      Serial.print("Received LoRa message: ");
      Serial.println((char*)buf);

      //Construct a forwarded message including Tower info.
      char forwardMessage[RH_RF95_MAX_MESSAGE_LEN];
      
      //Check if the received message has a "SRC:" field.
      char *srcPtr = strstr((char*)buf, "SRC:");
      if (srcPtr != NULL) {
        //Move pointer past "SRC:" to extract the sender's ID.
        srcPtr += 4; //points to the start of sender ID
        char clientID[10];  //Assume max 9-character ID
        int i = 0;
        while (srcPtr[i] != ';' && srcPtr[i] != '\0' && i < sizeof(clientID) - 1) {
          clientID[i] = srcPtr[i];
          i++;
        }
        clientID[i] = '\0';
        
        //If the sender is not the Tower, add an exclusion field.
        if (strcmp(clientID, TOWER_ID) != 0) {
          //Forward the message with an exclusion field indicating the originating client
          snprintf(forwardMessage, sizeof(forwardMessage), "SRC:%s;ORIG:%s;EXC:%s", TOWER_ID, (char*)buf, clientID);
        } else {
          //Message originates from the Tower (or an unknown source) - forward normally
          snprintf(forwardMessage, sizeof(forwardMessage), "SRC:%s;ORIG:%s", TOWER_ID, (char*)buf);
        }
      } else {
        //No "SRC:" field found, assume manual input)
        snprintf(forwardMessage, sizeof(forwardMessage), "SRC:%s;MSG:%s", TOWER_ID, (char*)buf);
      }
      
      Serial.print("Broadcasting message: ");
      Serial.println(forwardMessage);

      //Send broadcast message to all Clients
      driver.send((uint8_t*)forwardMessage, strlen(forwardMessage));
      driver.waitPacketSent();

      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  //Check if a user types a message in the Serial Monitor
  if (Serial.available()) {
    String userMessage = Serial.readStringUntil('\n');
    userMessage.trim(); 

    if (userMessage.length() > 0) {
      //Format: "SRC:<tower_id>;MSG:<user_message>"
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
