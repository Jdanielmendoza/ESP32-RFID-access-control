#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         22          // Pin del reset del RC522
#define SS_PIN          5         // Pin del SS (chip select) del RC522

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Crear instancia del RC522

void setup() {
  Serial.begin(9600);             // Iniciar comunicación serial
  SPI.begin();                    // Iniciar SPI bus
  mfrc522.PCD_Init();             // Iniciar el RC522
  Serial.println("Lector RFID-RC522 listo!");
}

void loop() {
  // Verificar si hay tarjetas presentes
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Obtener el UID de la tarjeta
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      uid.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.print("UID de la tarjeta: ");
    Serial.println(uid);
    Serial.println();
    mfrc522.PICC_HaltA();         // Detener la comunicación con la tarjeta
    mfrc522.PCD_StopCrypto1();    // Detener la encriptación
  }
}
