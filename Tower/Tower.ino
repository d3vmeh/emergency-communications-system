#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>
#include <math.h>



const int x_out = A2; /* connect x_out of module to A1 of UNO board */
const int y_out = A1; /* connect y_out of module to A2 of UNO board */
const int z_out = A0; /* connect z_out of module to A3 of UNO board */

//RF95 Setup
#define RF95_FREQ 915.0
RH_RF95 driver(8, 3); // Adafruit Feather M0 with RFM95 




int led = 13;

int count = -1;

int maxcount = 7; 



bool receivedID = false;

typedef struct Transmitter{
  char id[3]; //4 character code, starts with prefix "ID", followed by 2 digits for identification
  int emergencyButtonState = 0; //when 1, button is pressed. when 0, button unpressed
  int pitch;
  float tempC;
  float tempF;
  float humidity;
  int fallenState;
  int roll;
  
} Transmitter;


Transmitter helmet;



void setup() 
{
  
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  pinMode(led, OUTPUT);     
  Serial.begin(9600);



  //Radio Init

  //while (!Serial) ; // Wait for serial port to be available REMOVE OR UNCOMMENT WHEN NEEDED -- REQUIRES SERIAL MONITOR TO BE OPEN
  if (!driver.init())
    Serial.println("init failed");  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  if (!driver.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  driver.setTxPower(15, false);


  //For communicating with MSP432 via UART
  Serial1.begin(115200);
}

void loop()
{

  //Serial.println(potvalue);
  if (driver.available())
  {
    //Serial.println("avaialble");
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (driver.recv(buf, &len))
    {


      digitalWrite(led, HIGH);
      char s[len];
      strcpy(s, (char*)buf);  
      
      //Experimental
      if (receivedID){
        count += 1; 
        //Serial.println("INcrement: " + String(count));
      }


      Serial.print("Received message: ");
      strcpy(s, (char*)buf);
      Serial.println(s);

      //Check values
      if (count == 0 || count == -1){
        strcpy(s, (char*)buf);
        //id stored in s
        strcpy(helmet.id, s);
        if (count == -1){
          count = 0;
        }

        if (helmet.id[0] == 'I'){
          receivedID = true;
        }
      }

      // if (count == 1 && helmet.id[0] == 'I'){  
      //   char s[len];
      //   strcpy(s, (char*)buf);
      //   int n;
      //   n = atoi(s);
      //   //Serial.println(n);
      //   helmet.emergencyButtonState = n;
      //   }
      
      
      //Serial.println(count); // For debugging


      if (count > maxcount){
        count = -1;
      }

      // Send a reply
      uint8_t data[] = "Received data";
      driver.send(data, sizeof(data));
      driver.waitPacketSent();
      
       digitalWrite(led, LOW);
    }
    else
    {
      Serial.println("recv failed");
    }
  }

}