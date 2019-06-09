
/**
    Reciever code
*/

#include <SPI.h>
#include "RF24.h"

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(9, 10);
// This is the address used to send/receive
const byte addresses[][6] = {"00001", "00010"};
const int RELAY = 2;
void setup() {
  Serial.println(F("Reciever session starts here"));
  Serial.begin(9600);

  radio.begin();

  radio.enableAckPayload();                     // Allow optional ack payloads
  radio.enableDynamicPayloads();                // Ack payloads are dynamic payloads

  radio.openWritingPipe(addresses[0]);        // Both radios listen on the same pipes by default, but opposite addresses
  radio.openReadingPipe(1, addresses[1]);     // Open a reading pipe on address 0, pipe 1

  // Power setting. Due to likelihood of close proximity of the devices, set as RF24_PA_MIN (RF24_PA_MAX is default)
  radio.setPALevel(RF24_PA_MAX); // RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX

  // Slower data rate for better range
  radio.setDataRate(RF24_1MBPS); // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS

  // Number of retries and set tx/rx address
  radio.setRetries(15, 15);
}

void loop() {
  radio.startListening();
  byte pipeNo, gotByte;                          // Declare variables for the pipe and the byte received
  while ( radio.available(&pipeNo)) {            // Read all available payloads
    radio.read( &gotByte, sizeof( gotByte) );
    // Since this is a call-response. Respond directly with an ack payload.
    byte ack;
    if (checkDataIsValid(gotByte)) {
      doOperation(gotByte);
      ack  = generateAck(gotByte);
    } else {
      ack  = generateFailAck(gotByte);
    }
    // Ack payloads are much more efficient than switching to transmit mode to respond to a call
    radio.writeAckPayload(pipeNo, &ack, sizeof( ack) ); // This can be commented out to send empty payloads.

    Serial.print(F("Loaded next response "));
    Serial.println(gotByte);
    Serial.print(F("Ack generated"));
    Serial.println(ack);
  }
}

void doOperation(byte data) {
  if (data == 1) {
    digitalWrite(RELAY, LOW);
  } else if (data == 2) {
    digitalWrite(RELAY, HIGH);
  }
}

bool checkDataIsValid(byte data) {
  return (data == 1 || data == 2);
}

byte generateAck(byte data) {
  return (data | 0x80);
}

byte generateFailAck(byte data) {
  return (data | 0x40);
}

