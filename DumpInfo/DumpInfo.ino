// #include <SPI.h>
// #include <MFRC522.h>

// #define SS_PIN 5
// #define RST_PIN 22

// MFRC522 rfid(SS_PIN, RST_PIN);

// void setup() {
//   Serial.begin(115200);
//   SPI.begin(18, 19, 23);  // SCK, MISO, MOSI
//   rfid.PCD_Init();

//   Serial.println("Place your RFID card near the reader...");
// }

// void loop() {
//   if (!rfid.PICC_IsNewCardPresent()) {
//     return;
//   }
  
//   if (!rfid.PICC_ReadCardSerial()) {
//     return;
//   }

//   Serial.print("Tag UID: ");
//   for (byte i = 0; i < rfid.uid.size; i++) {
//     Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
//     Serial.print(rfid.uid.uidByte[i], HEX);
//   }
//   Serial.println();

//   rfid.PICC_HaltA();       // Stop reading
//   rfid.PCD_StopCrypto1();  // Stop encryption
// }


#define IR1 15
#define IR2 14

void setup() {
  Serial.begin(115200);

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);

  Serial.println("IR Sensors Ready...");
}

void loop() {

  int ir1State = digitalRead(IR1);
  int ir2State = digitalRead(IR2);

  // IR sensors usually give LOW when object detected
  if (ir1State == LOW) {
    Serial.println("IR1 detected something!");
  }

  if (ir2State == LOW) {
    Serial.println("IR2 detected something!");
  }

  delay(100);
}
