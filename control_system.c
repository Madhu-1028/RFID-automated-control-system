#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10        // SDA pin of RC522
#define RST_PIN 9        // RST pin of RC522
MFRC522 rfid(SS_PIN, RST_PIN);

Servo gateServo;

const int servoPin = 3;
const int openAngle = 90;     // Gate open angle
const int closeAngle = 0;     // Gate closed angle
const int openTime = 3000;    // Time to keep gate open (ms)

// Add authorized RFID tag IDs here
String authorizedTags[] = {
  "a1b2c3d4",   // Replace with your actual tag UID
  "12345678"
};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  gateServo.attach(servoPin);
  gateServo.write(closeAngle);
  Serial.println("RFID Gateway System Ready...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  String tagUID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    tagUID += String(rfid.uid.uidByte[i], HEX);
  }

  Serial.print("Scanned UID: ");
  Serial.println(tagUID);

  if (isAuthorized(tagUID)) {
    Serial.println("Access Granted. Opening Gate...");
    gateServo.write(openAngle);
    delay(openTime);
    Serial.println("Closing Gate...");
    gateServo.write(closeAngle);
  } else {
    Serial.println("Access Denied.");
  }

  delay(1000);  // debounce delay
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

bool isAuthorized(String tag) {
  for (String authorized : authorizedTags) {
    if (tag.equalsIgnoreCase(authorized)) {
      return true;
    }
  }
  return false;
}
