
/**
    Transmitter code
*/

#include <SPI.h>
#include "RF24.h"

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(9, 10);
// This is the address used to send/receive
const byte rxAddr[6] = "00001";

void setup() {
  Serial.println(F("Transmitter code"));
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(addresses[1]);        // Both radios listen on the same pipes by default, but opposite addresses
  radio.openReadingPipe(1, addresses[0]);     // Open a reading pipe on address 0, pipe 1

  radio.writeAckPayload(1, &counter, 1);
}

void loop() {
  radio.stopListening();
  byte gotByte;
  byte payLoad = getPayLoad();
  if ( radio.write(&payLoad, 1) ) {                       // Send the counter variable to the other radio
    if (!radio.available()) {                           // If nothing in the buffer, we got an ack but it is blank
      Serial.print(F("Got blank response. round-trip delay: "));
      Serial.print(micros() - time);
      Serial.println(F(" microseconds"));
    } else {
      while (radio.available() ) {                    // If an ack with payload was received
        radio.read( &gotByte, 1 );                  // Read it, and display the response time

        Serial.print(F("Got response "));
        Serial.print(gotByte);
        bool isAck = checkAck(gotByte);
        Serial.print(F("Response is %s", isAck ? "OK" : "FAIL"));
      }
    }
  } else {
    Serial.println(F("Can't send data!!!"));
  }
  delay(2000);
}

byte getPayLoad() {
  byte data = 0x1 & 0x1;
  return data;
}
bool checkAck(byte ack) {
  return (ack & 0x80) == 0x80
}

