#include <EEPROM.h> 

const int tempPin = A0;               
const int recordInterval = 5000; // 5 seconds     
const int maxDataAddr = 510;          
// Max EEPROM allowed for data w last 2 reserved for pointer
const int addrStoreLocation = 510;    // address for last 2

int addr = 0;                         
float tempC;                      

void setup() {
  Serial.begin(9600);                 
  addr = readAddress();              
  Serial.println("Temperature Logger Started");
  Serial.println("Commands: READ, CLEAR"); 
}

void loop() {
  // input handling
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); 
    command.trim(); // remove whitespaces
    if (command == "READ") {
      readEEPROMData(); // read and display all stored temps
    } else if (command == "CLEAR") {
      clearEEPROM(); // clear all data from EEPROM
    }
  }

  // if room to write another 2-byte temperature value
  if (addr + 2 <= maxDataAddr) {
    int rawReading = analogRead(tempPin); // read analog value from temperature sensor (0–1023)
    
    // raw reading to celsius using TMP36 formula
    tempC = ((rawReading * 5.0 / 1023.0) - 0.5) * 100.0;

    int tempInt = tempC * 100;            
    byte lowByte = tempInt & 0xFF; // lower 8 bits
    byte highByte = (tempInt >> 8) & 0xFF;// upper 8 bits

    EEPROM.update(addr, lowByte); // lower byte to EEPROM
    EEPROM.update(addr + 1, highByte); // upper byte to next EEPROM 
    addr += 2; // move 2 bytes
    saveAddress(addr);                   

    Serial.print("Logged: ");
    Serial.print(tempC, 1);              
    Serial.println(" C");
  } else {
    Serial.println("EEPROM FULL. Use CLEAR to erase.");
  }

  delay(recordInterval);
}

void readEEPROMData() {
  Serial.println("Reading EEPROM data...");
  for (int i = 0; i < addr; i += 2) { // read in 2-byte blocks
    byte low = EEPROM.read(i); // lower byte
    byte high = EEPROM.read(i + 1); // upper byte
    int tempRaw = (high << 8) | low; // combine to a single integer

    float temperature = tempRaw / 100.0; // back to float with two decimal places

    // skip extreme temps
    if (temperature < -40 || temperature > 125) continue;

    Serial.print("Reading ");
    Serial.print(i / 2);                  
    Serial.print(": ");
    Serial.print(temperature);
    Serial.println(" C");                 
  }
}

void clearEEPROM() {
  Serial.println("Clearing EEPROM...");
  for (int i = 0; i < maxDataAddr; i++) {
    EEPROM.update(i, 0xFF); // write blank value to each address
  }
  addr = 0; // reset write address
  saveAddress(addr);                     
  Serial.println("EEPROM Cleared.");
}

void saveAddress(int a) {
  EEPROM.update(addrStoreLocation, a & 0xFF);          
  EEPROM.update(addrStoreLocation + 1, (a >> 8) & 0xFF); 
}

int readAddress() {
  byte low = EEPROM.read(addrStoreLocation);           
  byte high = EEPROM.read(addrStoreLocation + 1);      
  return (high << 8) | low;                            
}
