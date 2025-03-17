#include <SPI.h>
#include <RH_RF95.h>
#include <math.h>

#define RF95_FREQ 915.0

// RF95 Setup
RH_RF95 driver(8, 3); // Adafruit Feather M0 with RFM95 

int count = 0;
bool idSent = false;

void setup() {
  Serial.begin(9600);
  if (!driver.init()) {
    Serial.println("init failed");
  }

  if (!driver.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);
  driver.setTxPower(15, false);

  // Serial1 is for communication with the Pi. ** UPDATE ** Probably won't need this
  Serial1.begin(115200);
  while (!Serial1) { }  // Wait for Serial connection
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  Serial.println("Sending to rf95_server: " + String(count));

  uint8_t msg[] = "Hello";
  Serial.println("Sending Message...");
  driver.send((uint8_t *)&msg, sizeof(msg));
  driver.waitPacketSent();
  Serial.println("Message sent\n");

  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (driver.waitAvailableTimeout(3000)) { 
    if (driver.recv(buf, &len)) {
      count += 1;
      Serial.print("Received msg from server: ");
      Serial.println((char*)buf);

      // **Forward message to Raspberry Pi**
      Serial.println((char*)buf);  

      // **Wait for acknowledgment from the Pi**
      delay(100);  // Give the Pi some time to respond
      if (Serial.available() > 0) {
        String ack = Serial.readStringUntil('\n');
        ack.trim();
        if (ack.length() > 0 && ack[0] == 'R') {
          Serial.println("Arduino Confirms Reception");
        }
      }
    } else {
      Serial.println("Receive failed");
    }
  } else {
    Serial.println("No reply, is rf95_server running?");
  }
  
  delay(1000);  // Adjust delay to prevent spam
}
