#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>
#include <stdlib.h>

#define RF95_FREQ 915.0

//Unique Client ID
const char CLIENT_ID[] = "CL01";


RH_RF95 driver(8, 3);


void setup() {
  Serial.begin(115200);  
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
  
  Serial.println("Client initialized");
}

void loop() {
  //Sending test heartbeat message every 5 sec
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    lastSend = millis();
    char msg[64];
    snprintf(msg, sizeof(msg), "SRC:%s;TYPE:HEARTBEAT;MSG:Hello from %s", CLIENT_ID, CLIENT_ID);
    
    Serial.print("Sending LoRa message: ");
    Serial.println(msg);
    
    driver.send((uint8_t*)msg, strlen(msg));
    driver.waitPacketSent();
  }
  
  //Listening from incoming messages from Tower
  if (driver.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (driver.recv(buf, &len)) {
      // Null-terminate the received message.
      if (len < RH_RF95_MAX_MESSAGE_LEN) {
        buf[len] = 0;
      } else {
        buf[RH_RF95_MAX_MESSAGE_LEN - 1] = 0;
      }
      
      Serial.print("Received LoRa message: ");
      Serial.println((char*)buf);
      

      Serial.println((char*)buf);
      
      //Code to process commands from Raspberry Pi
      
      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
      digitalWrite(LED_BUILTIN, LOW);
      
      Serial.println("Client confirms reception.");
    } else {
      Serial.println("LoRa recv failed");
    }
  }
  
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() > 0) {
      Serial.print("Received command from Pi: ");
      Serial.println(cmd);
    }
  }
  
  delay(100);
}
