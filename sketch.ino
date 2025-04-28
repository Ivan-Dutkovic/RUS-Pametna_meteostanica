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

// === Konstante za vrijeme ===
const unsigned long DISPLAY_DURATION = 15UL * 1000UL; ///< Trajanje prikaza u milisekundama (15 sekundi)
const unsigned long NOTIFY_INTERVAL = 30UL * 60UL * 1000UL;  ///< Interval za slanje obavijesti (30 minuta)
const unsigned long BUTTON_HOLD_THRESH = 1000UL;  ///< Prag za dugi pritisak tipkala (1 sekunda)

// === API ključevi i endpointi ===
const char* apiKey          = "7eff2e1a6f7addf0a8907c48759eb7ac"; ///< API ključ za OpenWeatherMap
const char* serverName      = "https://api.openweathermap.org/data/2.5/air_pollution"; ///< URL za očitavanje AQI podataka
const char* pushsaferKey    = "ucAAnFNENeakjzBcIM3z"; ///< Pushsafer API ključ
const char* pushsaferDevice = "92172"; ///< Pushsafer ID uređaja

// === DHT senzor postavke ===
#define DHTPIN 4 ///< Pin za DHT senzor
#define DHTTYPE DHT22 ///< Tip DHT senzora
DHT dht(DHTPIN, DHTTYPE);

// === OLED zaslon postavke ===
#define OLED_RESET -1 ///< Nema reset pina za OLED
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

// === Varijable za stanje i vrijeme ===
bool initialDone = false; ///< Je li početno očitanje završeno
bool displayIsOn = false; ///< Je li zaslon uključen
unsigned long lastNotifyTime = 0; ///< Vrijeme posljednje obavijesti
unsigned long displayStartTime = 0; ///< Vrijeme kada je zaslon uključen

// === Očitavanja senzora ===
float temperature; ///< Temperatura u stupnjevima Celzija
float humidity; ///< Vlažnost zraka u postocima
int lightIntensity; ///< Intenzitet svjetla (analogno očitanje)
int aqi; ///< Indeks kvalitete zraka

const int buttonPin = 15; ///< Pin tipkala
const unsigned long debounceDelay = 50; ///< Debounce vrijeme (50ms)
const unsigned long longPressTime = 1000; ///< Vrijeme za prepoznavanje dugog pritiska (1 sekunda)

bool buttonState = HIGH; ///< Trenutno stanje tipkala
bool lastButtonState = HIGH; ///< Prethodno stanje tipkala
unsigned long lastDebounceTime = 0; ///< Vrijeme posljednje promjene stanja tipkala
unsigned long buttonPressStartTime = 0; ///< Vrijeme početka pritiska tipkala
bool isPressed = false; ///< Je li tipkalo pritisnuto

const int lineSpacing = 12; ///< Razmak između linija na OLED ekranu

// === Deklaracije funkcija ===

/**
 * @brief Ažurira očitanja sa senzora i API-ja.
 */
void updateReadings();

/**
 * @brief Isključuje zaslon.
 */
void turnOffDisplay();

/**
 * @brief Uključuje zaslon.
 */
void turnOnDisplay();

/**
 * @brief Vraća opis razine svjetlosti na temelju analognog očitanja.
 * @param analogValue Analogna vrijednost svjetla.
 * @return Opis razine svjetlosti.
 */
String getLightLevelDescription(int analogValue);

/**
 * @brief Vraća opis AQI razine.
 * @param aqi Vrijednost AQI.
 * @return Opis razine AQI.
 */
String getAQIDescription(int aqi);

/**
 * @brief Prikazuje očitane podatke na zaslonu.
 */
void displaySensorData();

/**
 * @brief Ažurira podatke i prikazuje ih na zaslonu.
 */
void updateAndDisplaySensorData();

/**
 * @brief Ažurira podatke i šalje Pushsafer obavijest.
 */
void updateAndNotify();

/**
 * @brief Šalje Pushsafer obavijest sa zadanim podacima.
 * @param temperature Temperatura u °C.
 * @param humidity Vlažnost zraka u %.
 * @param lightIntensity Intenzitet svjetla.
 * @param aqi Indeks kvalitete zraka.
 */
void sendPushsaferNotification(float temperature, float humidity, int lightIntensity, int aqi);

/**
 * @brief Dohvaća trenutno lokalno vrijeme formatirano kao HH:MM.
 * @return String s formatiranim vremenom.
 */
String getFormattedTime();

/**
 * @brief Funkcija koja se izvršava prilikom pokretanja programa.
 */
void setup() {
  Serial.begin(9600);

  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  pinMode(buttonPin, INPUT_PULLUP);

  WiFi.begin("Wokwi-GUEST", "", 6);
  Serial.print("Povezivanje na WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Uspješno povezan!");

  configTime(3600, 3600, "pool.ntp.org");
  Serial.println("Pokretanje sinkronizacije vremena...");

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Čekanje vremena...");
    delay(500);
  }
  Serial.println("Vrijeme sinkronizirano!");
}

/**
 * @brief Glavna petlja programa.
 */
void loop() {
  unsigned long now = millis();

  if (!initialDone) {
    updateAndDisplaySensorData();
    sendPushsaferNotification(temperature, humidity, lightIntensity, aqi);
    initialDone = true;
    displayStartTime = now;
    lastNotifyTime = now;
    displayIsOn = true;
  }

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        buttonPressStartTime = millis();
        isPressed = true;
      } else if (buttonState == HIGH && isPressed) {
        unsigned long pressDuration = millis() - buttonPressStartTime;
        if (pressDuration < longPressTime) {
          Serial.println("Kratki pritisak tipkala");
          updateAndDisplaySensorData();
          displayStartTime = now;
          displayIsOn = true;
        } else {
          Serial.println("Dugi pritisak tipkala");
          updateAndNotify();
        }
        isPressed = false;
      }
    }
  }

  lastButtonState = reading;

  if (displayIsOn && (now - displayStartTime >= DISPLAY_DURATION)) {
    turnOffDisplay();
    displayIsOn = false;
  }

  if (now - lastNotifyTime >= NOTIFY_INTERVAL) {
    updateAndNotify();
    lastNotifyTime = now;
  }
}


void updateReadings() {
  do {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();  
    if (isnan(temperature) || isnan(humidity)) {
      delay(100);
    }
  } while (isnan(temperature) || isnan(humidity)); 
  int temperatureInt = (int)temperature;
  int humidityInt = (int)humidity;

  lightIntensity = analogRead(32);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String path = String(serverName) + "?lat=45.8150&lon=15.9819&appid=" + apiKey;
    http.begin(path);
    int code = http.GET();
    if (code > 0) {
      StaticJsonDocument<1024> doc;
      auto err = deserializeJson(doc, http.getString());
      if (!err) {
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

void updateAndDisplaySensorData() {
  updateReadings();
  displaySensorData();
  turnOnDisplay();
}

void updateAndNotify() {
  updateReadings();
  sendPushsaferNotification(temperature, humidity, lightIntensity, aqi);
}

void sendPushsaferNotification(float temperature, float humidity, int lightIntensity, int aqi) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String message = "Temperatura: " + String(temperature, 1) + "°C\n";
    message       += "Vlaga: "       + String(humidity, 1)    + "%\n";
    message       += "Svjetlost: "   + getLightLevelDescription(lightIntensity) + "\n";
    message       += "AQI: "         + getAQIDescription(aqi);
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
