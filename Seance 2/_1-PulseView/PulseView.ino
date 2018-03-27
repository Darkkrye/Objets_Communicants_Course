#include <SPI.h>

const int chipSelectPin = 7;

void setup() {

  SPI.begin();

  pinMode(chipSelectPin, OUTPUT);
  delay(100);
}

void loop() {
  
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(0xDE);
  SPI.transfer(0xAD);

  digitalWrite(chipSelectPin, HIGH);

  SPI.endTransaction();

  delay(300);
}
