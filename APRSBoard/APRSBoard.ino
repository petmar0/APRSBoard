/*
APRS Board Code
11/11/2014 Pete Marchetto pmm223@cornell.edu
This program is designed to be used with a Bell202 style modem, and to be fully compatible with the APRS AX.25 protocol.
The analog values are read from all connected channels, then sent in a comma separated list through an APRS packet.
All relevant data is also parroted to the serial port, which should host an OpenLog logger.

Standard beerware license applies: if you find this useful and meet me, you owe me a beer.
*/

#include <Time.h>  //Include Time library for timestamping
#include <SoftwareSerial.h>  //Include SoftwareSerial library for communication with modem
#define callsign "KPMMA1"  //Define callsign; this must be <=6 chars
#define path "WIDE3-3"  //Repeater path; in most cases this will be WIDE3-3
#define destination "TLM"  //Destination is telemetry, TLM; TEL can also be used if preferred
#define ver "APZ001"  //APRS Version: APZ is experimental terminal type, 001 is the version number
#define type "@"  //APRS station type; @ is a timestamped location
#define loc "4226.445N/07628.171W"  //Location in mindec separated by a slash; degree is the leading 2-3 digits
#define period 1  //Data transmission delay in minutes

SoftwareSerial Bell202(2,3);  //Set up the radio modem on pins 2 and 3
int minin=minute();  //Define minin start time

void setup()  {
  Serial.begin(9600);  //Start serial session with OpenLog @ 9600 bps
  Bell202.begin(1200);  //Start serial session with the modem @ 1200 bps
  pinMode(4, OUTPUT);  //M0 pin of modem: transmit enable
  pinMode(5, OUTPUT);  //M1 pin of modem: receive enable
  pinMode(13, OUTPUT);  //Arduino LED, used as transmit indicator
}

void loop(){
  if(minute()-minin==period){  //If the time is one period later than the last transmission, or if it's the same minute, then transmit
    digitalWrite(4, HIGH);  //Set M0 to transmit
    digitalWrite(5, LOW);  //Set M1 to transmit
    digitalWrite(13, HIGH);  //Indicate transmission with pin 13 LED
    Bell202.print("~");  //Send flag character to modem
    Bell202.print(destination);  //Send destination to modem
    Bell202.print(ver);  //Send version to modem
    Bell202.print(callsign);  //Send callsign to modem
    Bell202.print(path);  //Send path to modem
    Bell202.print(0x03);  //Send control character to modem
    Bell202.print(0xF0);  //Send protocol ID to modem
    Bell202.print(type);  //Send data type to modem
    Bell202.print(day());  //Send day part of DHM to modem
    Serial.print(day());  //Send day part of DHM to logger
    Bell202.print(hour());  //Send hour part of DHM to modem
    Serial.print(hour());  //Send hour part of DHM to logger
    Bell202.print(minute());  //Send minute part of DHM to modem
    Serial.print(minute());  //Send minute part of DHM to logger
    Bell202.print("z");  //Send UTC indicator to modem
    Serial.print(",");  //Send comma separator to logger
    Bell202.print(loc);  //Send location to modem
    Serial.print(loc);  //Send location to logger
    Serial.print(",");
    Bell202.print(".");  //Send . SSID "X" to modem
    for(int i=0; i<8; i++){  //Cycle through all 8 analog channels once
      Bell202.print(analogRead(i));  //Send analog value to modem
      Serial.print(analogRead(i));  //Send analog value to logger
      Bell202.print(",");  //Send comma separator to modem
      Serial.print(",");  //Send comma separator to logger
    }
    Bell202.print("~");  //Send flag character to modem
    Serial.println();  //Send EOL to logger
    digitalWrite(4, LOW);  //Set M0 to receive
    digitalWrite(5, HIGH);  //Set M1 to receive
    digitalWrite(13, LOW);  //Turn off indicator LED
  }
  char preamble=Bell202.read();  //Get a character from the modem
  if(preamble=='~'){  //If the preamble character is a flag
    char msg[333];  //Create a character array for the incoming message
    for(int j=0; j<333; j++){  //Cycle through the array
      msg[j]=Bell202.read();  //Populate the array with input from the modem
    }
    digitalWrite(4, HIGH);  //Set M0 to transmit
    digitalWrite(5, LOW);  //Set M1 to transmit
    digitalWrite(13, HIGH);  //Indicate transmission with pin 13 LED
    Bell202.print("~");  //Send flag character
    for(int j=0; j<333; j++){  //Cycle through the array
      Bell202.print(msg[j]);  //Write the array to the modem
    }
    Bell202.print("~");  //Send flag character
    digitalWrite(4, LOW);  //Set M0 to receive
    digitalWrite(5, HIGH);  //Set M1 to receive
    digitalWrite(13, LOW);  //Turn off indicator LED
  }
  minin=minute();  //Define the minute when the first loop ends
}
