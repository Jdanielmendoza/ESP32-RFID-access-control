//----------------------------------------------------------------------------------------
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#define USE_SERIAL Serial
WiFiMulti wifiMulti;

//----------------------------------------------------------------------------------------
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN 22                 // Configurable, see typical pin layout above
#define SS_PIN 5                   // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
//----------------------------------------------------------------------------------------
#define ROJO 14
#define VERDE 12

void setup() {
  pinMode(ROJO, OUTPUT);
  pinMode(VERDE, OUTPUT);

  USE_SERIAL.begin(115200);

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[Configurando] Espere %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
  wifiMulti.addAP("6c9102", "240488271");
  wifiMulti.addAP("Lab236-35", "Lab_236@36");

  //------------------------------------------------------------------------------------------------------
  SPI.begin();                        // Init SPI bus
  mfrc522.PCD_Init();                 // Init MFRC522
  delay(10);                          // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  USE_SERIAL.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  //------------------------------------------------------------------------------------------------------
}

void loop() {
  // wait for WiFi connection
  if ((wifiMulti.run() == WL_CONNECTED)) {

    if (!mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    //leer el contenido de la tarjeta y almacenarlo en la variale uid
    String uidString = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uidString += String(mfrc522.uid.uidByte[i], HEX);
    }
    unsigned long uid = strtoul(uidString.c_str(), NULL, 16);
    USE_SERIAL.println("UID leido: " + uidString);
    USE_SERIAL.println("UID almacenado en la variable 'uid': " + String(uid));

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");
    String url = "http://192.168.0.19:3000/" + String(uidString);
    http.begin(url);

    USE_SERIAL.print("[HTTP] GET...\n");
    int httpCode = http.GET();

    if (httpCode > 0) {
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        if (payload == "activo") {
          USE_SERIAL.println("acceso permitido");
          digitalWrite(VERDE, HIGH);
          delay(4000);
          digitalWrite(VERDE, LOW);

        } else if (payload == "inactivo") {
          USE_SERIAL.println("acceso denegado!");
          digitalWrite(ROJO, HIGH);
          delay(4000);
          digitalWrite(ROJO, LOW);
        } else {
          USE_SERIAL.println("uid no valido! o no existe");
        }
        USE_SERIAL.println(payload);
      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();

    delay(2000);
  }
}
