/**
 * @file sketch.ino
 * @brief Glavna datoteka programa za očitavanje senzora, prikaz podataka i slanje obavijesti.
 */

 #include <WiFi.h>
 #include <HTTPClient.h>
 #include <ArduinoJson.h>
 #include <Wire.h>
 #include <Adafruit_Sensor.h>
 #include <DHT.h>
 #include <Adafruit_GFX.h>
 #include <Adafruit_SSD1306.h>
 #include <time.h>
 #include <esp_sleep.h>               // Za deep sleep i wakeup izvore
 #include "driver/rtc_io.h"
 
 // === Konstante za vrijeme ===
 const unsigned long DISPLAY_DURATION   = 15UL * 1000UL; ///< Trajanje prikaza u milisekundama (15 sekundi)
 const unsigned long BUTTON_HOLD_THRESH = 1000UL;        ///< Prag za dugi pritisak tipkala (1 sekunda)
 
 // === API ključevi i endpointi ===
 const char* apiKey          = "7eff2e1a6f7addf0a8907c48759eb7ac"; ///< API ključ za OpenWeatherMap
 const char* serverName      = "https://api.openweathermap.org/data/2.5/air_pollution"; ///< URL za očitavanje AQI podataka
 const char* pushsaferKey    = "ucAAnFNENeakjzBcIM3z"; ///< Pushsafer API ključ
 const char* pushsaferDevice = "92172"; ///< Pushsafer ID uređaja
 
 // === DHT senzor postavke ===
 #define DHTPIN 4    ///< Pin za DHT senzor
 #define DHTTYPE DHT22///< Tip DHT senzora
 DHT dht(DHTPIN, DHTTYPE);
 
 // === OLED zaslon postavke ===
 #define OLED_RESET -1 ///< Nema reset pina za OLED
 Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);
 
 // === Varijable za očitanja ===
 float temperature; ///< Temperatura u °C
 float humidity;    ///< Vlažnost u %RH
 int   lightIntensity; ///< Analogno očitanje svjetla
 int   aqi;            ///< Air Quality Index
 
 // === Tipkalo ===
 const int buttonPin = 15; ///< Pin tipkala (RTC IO)
 bool  lastButtonState;    ///< za debouncing
 
 // === Razmak između linija na ekranu ===
 const int lineSpacing = 12;
 
 // === Ovo živi u RTC memoriji da preživi deep sleep ===
 RTC_DATA_ATTR time_t nextWakeEpoch = 0; 
 
 // === Deklaracije funkcija ===
 void updateReadings();
 void turnOffDisplay();
 void turnOnDisplay();
 String getLightLevelDescription(int analogValue);
 String getAQIDescription(int aqi);
 void displaySensorData();
 void sendPushsaferNotification(float temperature, float humidity, int lightIntensity, int aqi);
 String getFormattedTime();
 
 void setup() {
   Serial.begin(9600);
 
   // --- inicijalizacija senzora i zaslona ---
   dht.begin();
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(WHITE);
 
   pinMode(buttonPin, INPUT_PULLUP);
 
   // --- spoj na WiFi ---
   WiFi.begin("Wokwi-GUEST", "", 6);
   Serial.print("Povezivanje na WiFi");
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }
   Serial.println(" Uspješno povezan!");
 
   // --- NTP za epoch time ---
   configTime(3600, 3600, "pool.ntp.org");
   Serial.println("Sinkronizacija vremena...");
   struct tm timeinfo;
   while (!getLocalTime(&timeinfo)) {
     Serial.print(".");
     delay(500);
   }
   Serial.println(" Vrijeme sinkronizirano!");
 
   // --- određivanje uzroka buđenja ---
   esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
   time_t nowEpoch = time(nullptr);
 
   if (nextWakeEpoch == 0) {
     // === Prvi startup ===
     updateReadings();
     displaySensorData();
     turnOnDisplay();
     sendPushsaferNotification(temperature, humidity, lightIntensity, aqi);
     delay(DISPLAY_DURATION);
     turnOffDisplay();
     nextWakeEpoch = nowEpoch + 30 * 60; // za 30 minuta
   }
   else if (cause == ESP_SLEEP_WAKEUP_TIMER) {
     // === Automatsko buđenje timerom ===
     updateReadings();
     sendPushsaferNotification(temperature, humidity, lightIntensity, aqi);
     nextWakeEpoch = nowEpoch + 30 * 60;
   }
   else if (cause == ESP_SLEEP_WAKEUP_EXT0) {
     // === Buđenje tipkалом ===
     // pričekaj otpuštanje i izmjeri trajanje držanja
     unsigned long start = millis();
     while (digitalRead(buttonPin) == LOW) {
       delay(10);
     }
     unsigned long dur = millis() - start;
     if (dur < BUTTON_HOLD_THRESH) {
       // kratki pritisak → prikaži podatke 15s
       updateReadings();
       displaySensorData();
       turnOnDisplay();
       delay(DISPLAY_DURATION);
       turnOffDisplay();
     } else {
       // dugi pritisak → samo notifikacija
       updateReadings();
       sendPushsaferNotification(temperature, humidity, lightIntensity, aqi);
     }
     // nextWakeEpoch ostaje nepromijenjen
   }
   else {
     // nepoznat razlog (npr. hladno pokretanje) → tretiraj kao prvi
     updateReadings();
     displaySensorData();
     turnOnDisplay();
     sendPushsaferNotification(temperature, humidity, lightIntensity, aqi);
     delay(DISPLAY_DURATION);
     turnOffDisplay();
     nextWakeEpoch = nowEpoch + 30 * 60;
   }
 
   // --- izračunaj koliko još sekundi do sljede automatske notifikacije ---
   time_t now2 = time(nullptr);
   long  remaining = nextWakeEpoch - now2;
   if (remaining <= 0) {
     remaining = 30 * 60;
     nextWakeEpoch = now2 + remaining;
   }
 
   // --- postavi wakeup izvore ---
   // timer za preostalo vrijeme
   esp_sleep_enable_timer_wakeup(uint64_t(remaining) * 1000000ULL);
   // eksterno buđenje na LOW na pin 15
   esp_sleep_enable_ext0_wakeup(GPIO_NUM_15, 0);
 
   // --- idemo na deep sleep! ---
   Serial.printf("Spavam još %ld sekundi...\n", remaining);
   esp_deep_sleep_start();
 }
 
 void loop() {
   // nikad se ne poziva, jer setup() uvijek završi deep sleepom
 }
 
 void updateReadings() {
   do {
     temperature = dht.readTemperature();
     humidity    = dht.readHumidity();  
     if (isnan(temperature) || isnan(humidity)) {
       delay(100);
     }
   } while (isnan(temperature) || isnan(humidity));
 
   lightIntensity = analogRead(32);
 
   if (WiFi.status() == WL_CONNECTED) {
     HTTPClient http;
     String path = String(serverName)
                 + "?lat=45.8150&lon=15.9819&appid=" + apiKey;
     http.begin(path);
     int code = http.GET();
     if (code > 0) {
       StaticJsonDocument<1024> doc;
       if (!deserializeJson(doc, http.getString())) {
         JsonArray list = doc["list"].as<JsonArray>();
         if (list.size()) {
           aqi = list[0]["main"]["aqi"] | 0;
         }
       }
     }
     http.end();
   }
 }
 
 void turnOffDisplay() {
   display.ssd1306_command(SSD1306_DISPLAYOFF);
 }
 
 void turnOnDisplay() {
   display.ssd1306_command(SSD1306_DISPLAYON);
 }
 
 String getLightLevelDescription(int analogValue) {
   if      (analogValue > 1000) return "Pun mjesec";
   else if (analogValue > 950)  return "Zora";
   else if (analogValue > 800)  return "Sumrak";
   else if (analogValue > 600)  return "Monitor računala";
   else if (analogValue > 500)  return "Stepenište";
   else if (analogValue > 200)  return "Uredsko osvjetljenje";
   else if (analogValue > 100)  return "Oblačan dan";
   else if (analogValue > 20)   return "Dnevna svjetlost";
   else                         return "Izravna sunčeva svjetlost";
 }
 
 String getAQIDescription(int aqi) {
   switch (aqi) {
     case 1: return "Dobra";
     case 2: return "Umjerena";
     case 3: return "Nezdrava za osjetljive skupine";
     case 4: return "Nezdrava";
     case 5: return "Vrlo nezdrava";
     default: return "Nepoznato";
   }
 }
 
 void displaySensorData() {
   display.clearDisplay();
   display.setCursor(0, 0);
   display.println("Vrijeme: " + getFormattedTime());
   display.setCursor(0, lineSpacing);
   display.println("Temperatura: " + String(temperature) + " C");
   display.setCursor(0, 2 * lineSpacing);
   display.println("Vlaga: " + String(humidity) + " %");
   display.setCursor(0, 3 * lineSpacing);
   display.println("Svjetlost: " + getLightLevelDescription(lightIntensity));
   display.setCursor(0, 4 * lineSpacing);
   display.println("AQI: " + getAQIDescription(aqi));
   display.display();
 }
 
 void sendPushsaferNotification(float temperature, float humidity, int lightIntensity, int aqi) {
   if (WiFi.status() == WL_CONNECTED) {
     HTTPClient http;
     String message = "Temperatura: " + String(temperature, 1) + "°C\n";
     message      += "Vlaga: "     + String(humidity, 1)    + "%\n";
     message      += "Svjetlost: " + getLightLevelDescription(lightIntensity) + "\n";
     message      += "AQI: "       + getAQIDescription(aqi);
     message.replace(" ", "%20");
     message.replace("°", "%C2%B0");
     message.replace("\n", "%0A");
     String url = "https://www.pushsafer.com/api?k=" + String(pushsaferKey)
                + "&d=" + String(pushsaferDevice)
                + "&m=" + message;
     http.begin(url);
     int code = http.GET();
     if (code > 0) {
       Serial.println("Poruka uspješno poslana!");
     } else {
       Serial.print("Greška prilikom slanja poruke: ");
       Serial.println(code);
     }
     http.end();
   } else {
     Serial.println("WiFi nije povezan. Ne mogu poslati obavijest.");
   }
 }
 
 String getFormattedTime() {
   struct tm timeinfo;
   if (!getLocalTime(&timeinfo)) {
     return "Nema vremena";
   }
   char buffer[20];
   strftime(buffer, sizeof(buffer), "%H:%M", &timeinfo);
   return String(buffer);
 }
 
