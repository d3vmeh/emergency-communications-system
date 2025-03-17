#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>
#include <stdlib.h>

#define RF95_FREQ 915.0

//Unique Tower ID
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
  
  Serial.println("Tower initialized");
}

void loop() {
  //Checking if message is available from any Client
  if (driver.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (driver.recv(buf, &len)) {
      if (len < RH_RF95_MAX_MESSAGE_LEN) {
        buf[len] = 0;
      } else {
        buf[RH_RF95_MAX_MESSAGE_LEN - 1] = 0;
      }
      
      Serial.print("Received LoRa message: ");
      Serial.println((char*)buf);
      
      //Forwarding message to clients
      char forwardMessage[RH_RF95_MAX_MESSAGE_LEN];
      snprintf(forwardMessage, sizeof(forwardMessage), "SRC:%s;ORIG:%s", TOWER_ID, (char*)buf);
      
      Serial.print("Broadcasting message: ");
      Serial.println(forwardMessage);
      
      //Sending to all clients
      driver.send((uint8_t*)forwardMessage, strlen(forwardMessage));
      driver.waitPacketSent();
      
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      Serial.println("LoRa recv failed");
    }
  }
  
  delay(100);
}
