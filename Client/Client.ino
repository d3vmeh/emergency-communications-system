// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

#include <SPI.h>
#include <RH_RF95.h>
#include <math.h>


#define RF95_FREQ 915.0


//RF95 Setup
RH_RF95 driver(8, 3); // Adafruit Feather M0 with RFM95 

int count = 0;




void sendStr(char* data[20]){


}

void sendInt(int num){
  char a[20];
  itoa(num, a, 10);
  driver.send((uint8_t*)a, sizeof(a));
  Serial.println(a);
  driver.waitPacketSent();
}



bool idSent = false;
void setup() 
{

  Serial.begin(9600);
  //while (!Serial) ; // Wait for serial port to be available
  if (!driver.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  if (!driver.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);


  driver.setTxPower(15, false);
}

void loop()
{

  Serial.println("Sending to rf95_server" + String(count));
  //Serial.println(digitalRead(BUTTON_PIN));
  // Send a message to rf95_server
  // if (count == 0){
  //   Serial.println("Sending id");
  //   uint8_t data[] = "ID01";
  // //Serial.println(&data[0]));
  //   driver.send(data, sizeof(data));
  // //sendInt(1000);
  //   idSent = true;
  // }


/*
  Serial.println("TEMP: " + String(temp_c));
  Serial.println(digitalRead(BUTTON_PIN));
    Serial.println(" potvalue: " + String(analogRead(A3)));
*/

  Serial.println("COUNT: " + String(count));

  uint8_t msg[] = "Hello";
  Serial.println("Sending Message: ");
  //Serial.println(msg);

  //static char *msg = msg.c_str();
  driver.send((uint8_t *)&msg, sizeof(msg));
  driver.waitPacketSent();
  Serial.println("Message sent\n\n");
  //sendInt(msg);

  
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);



  if (driver.waitAvailableTimeout(3000))
  { 
    // Should be a reply message for us now   
    if (driver.recv(buf, &len))
   {
      count += 1;

      Serial.print("got reply: ");
      Serial.println((char*)buf);

    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }
  delay(100);
}