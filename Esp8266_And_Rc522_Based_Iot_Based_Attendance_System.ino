

#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define SS_PIN  D4
#define RST_PIN D3
int yellow = D0;
int buzzer = D8;

MFRC522 mfrc522(SS_PIN, RST_PIN);

const char *ssid = "JustDo";
const char *password = "par12345";
const char* device_token  = "c9f370e203924121";

String URL = "http://192.168.43.9/rfidattendance/getdata.php";
String getData, Link;
String OldCardID = "";
unsigned long previousMillis = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  pinMode(yellow, OUTPUT);
  pinMode(buzzer, OUTPUT);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Attendance System");

  connectToWiFi();
}

void loop() {

  if (!WiFi.isConnected()) {
    connectToWiFi();
  }

  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID = "";
  }
  delay(50);

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String CardID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }

  if ( CardID == OldCardID ) {
    return;
  }
  else {
    OldCardID = CardID;
  }

  SendCardID(CardID);
  delay(1000);
}

void SendCardID( String Card_uid ) {
  Serial.println("Sending the Card ID");
  if (WiFi.isConnected()) {
    HTTPClient http;
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token);

    Link = URL + getData;
    http.begin(Link);

    int httpCode = http.GET();
    String payload = http.getString();


    Serial.println(httpCode);
    Serial.println(Card_uid);
    digitalWrite(yellow, HIGH);
    digitalWrite(buzzer, HIGH);
    delay(2000);
    digitalWrite(buzzer, LOW);
    digitalWrite(yellow, LOW);
    lcd.setCursor(0, 2);
    lcd.print("Your ID");
    lcd.setCursor(9, 2);
    lcd.print(Card_uid);
    delay(2000);
    lcd.clear();
    lcd.setCursor(3, 1);
    lcd.print("Your Attendance");
    lcd.setCursor(5, 2);
    lcd.print("Is Complted");
    
    delay(2000);
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Attendance System");

    Serial.println(payload);

    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);


      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);


      }
      else if (payload == "succesful") {

      }
      else if (payload == "available") {

      }
      delay(100);
      http.end();
    }
  }
}

void connectToWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1000);
}
