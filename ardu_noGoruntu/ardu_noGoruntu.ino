#include <Servo.h>
#include <FaBoLCD_PCF8574.h>
#include <Wire.h>
#include <DHT.h>


// LDR sensör pinlerini tanımla
#define LDR1 A0
#define LDR2 A1

// Hata değerini tanımla
#define error 10
//sicaklik
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

FaBoLCD_PCF8574 lcd;


// Servo motorun başlangıç noktası
int Spoint = 90;

// Servo motor için bir nesne oluştur
Servo servo;
int relayPin = 13; // Röle pini

void setup() {
  lcd.begin(16,2);
  lcd.print("Veriler aliniyor...");
  delay(1000);
  dht.begin();

  // Servo motoru PWM pinine bağla
  servo.attach(11);
  // Servo motorun başlangıç noktasını ayarla
  servo.write(Spoint);
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Başlangıçta rölenin kapalı olduğundan emin ol
}

void loop() {
  int temp = dht.readTemperature();
  int hum = dht.readHumidity();

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Sicaklik: ");
  lcd.print(temp);
  lcd.print(" C");

  lcd.setCursor(0,1);
  lcd.print("Nem: %");
  lcd.print(hum);

  // LDR sensör değerlerini al
  int ldr1 = analogRead(LDR1);
  int ldr2 = analogRead(LDR2);

  // Bu değerlerin farkını al
  int value1 = abs(ldr1 - ldr2);

  // Bu değerleri bir koşullu ifadeyle kontrol et
  if (value1 > error) {
    if (ldr1 > ldr2 && Spoint > 10) {
      Spoint--;
    } else if (ldr1 < ldr2 && Spoint < 160) {
      Spoint++;
    }
  }

  // Servo motora değerleri yaz0
  servo.write(Spoint);
  delay(80);
  Serial.print("LDR1:");
  Serial.println(ldr1);
  Serial.print("LDR2:");
  Serial.println(ldr2);

  // Seri girişe dayalı olarak röle kontrolünü ele al
  if (Serial.available() > 0) {
    char receivedChar = Serial.read(); // Gelen baytı oku
    if (receivedChar == '1' ||receivedChar == -1  ) { // Eğer '1' gelirse
      digitalWrite(relayPin, LOW); // Röleyi aç
      delay(500);
    } else if (receivedChar == '2') { // Eğer '2' gelirse
      digitalWrite(relayPin, HIGH); // Röleyi aç
      delay(500); // 2 saniye bekle
    } 
  } else {
    digitalWrite(relayPin, HIGH); // Seri giriş yoksa rölenin kapalı olduğundan emin ol
  }
}
