#  Pametna meteostanica
https://wokwi.com/projects/426855239853016065

## 📌 Opis projekta

Projekt **pametna meteostanica** omogućuje praćenje vremenskih uvjeta u realnom vremenu kroz kombinaciju senzora i IoT funkcionalnosti.  
**Cilj:** Pružiti precizno mjerenje okoline, energetski efikasno rješenje s mogućnošću daljinskog nadzora.

### Ključne značajke

- 🌡️ Mjerenje temperature i vlažnosti zraka (DHT22 senzor)
- ☀️ Detekcija intenziteta svjetlosti (fotootpornik)
- 📺 Prikaz podataka na OLED zaslonu (uključujući temperaturu, vlažnost, svjetlost, vrijeme i kvalitetu zraka)
- 🕒 Prikaz trenutnog vremena (sinkronizirano putem interneta)
- 🌫️ Prikaz kvalitete zraka dohvaćene putem API-ja
- 💤 Energetski efikasni način rada (duboki san)
- 🔘 Interakcija preko tipke za buđenje sustava
- 📲 Automatske notifikacije na mobilnu aplikaciju

---

## 🔌 Komponente

| Komponenta             | Opis |
|------------------------|------|
| **ESP32**              | Glavni mikrokontroler s WiFi mogućnostima |
| **DHT22**              | Mjeri temperaturu i vlažnost zraka |
| **OLED zaslon (128x64)** | Prikazuje podatke u realnom vremenu putem I2C komunikacije |
| **Fotootpornik**       | Mjeri intenzitet ambijentalnog svjetla |
| **Tipka**              | Koristi se za ručno buđenje sustava i slanje notifikacija |
| **Otpornici**          | Povezani s tipkom i fotootpornikom za stabilnost signala |

---

## ✅ Funkcionalni zahtjevi

| ID       | Opis |
|----------|------|
| **FR-1**  | Sustav mora mjeriti temperaturu i vlažnost zraka pomoću **DHT22** senzora. |
| **FR-2**  | Sustav mora mjeriti intenzitet ambijentalnog svjetla pomoću **fotootpornika**. |
| **FR-3**  | Izmjerene vrijednosti temperature i vlage moraju se prikazati na **OLED zaslonu**. |
| **FR-4**  | Sustav mora prikazati **kvalitetu zraka** na OLED zaslonu, dohvaćenu putem **API-ja**. |
| **FR-5**  | Na **OLED zaslonu** mora se prikazivati i trenutno **vrijeme**. |
| **FR-6**  | Nakon prikaza podataka, sustav mora automatski ući u **duboki san** radi uštede energije. |
| **FR-7**  | Sustav mora omogućiti **buđenje iz dubokog sna** pomoću tipke. |
| **FR-8**  | Kratkim pritiskom tipke, sustav se budi i prikazuje trenutno očitane podatke. |
| **FR-9**  | Dugim pritiskom tipke (dulje od 1 sekunde), sustav mora ponovno poslati **obavijest korisniku** putem HTTP POST zahtjeva. |
| **FR-10** | Svakih **30 minuta**, sustav se mora automatski probuditi i poslati **notifikaciju** korisniku putem HTTP POST zahtjeva s trenutno izmjerenim podacima. |
| **FR-11** | Sustav mora koristiti **millis()** umjesto **delay()** gdje god je moguće za neblokirajuće operacije. |
| **FR-12** | Prikaz podataka na OLED zaslonu mora se automatski isključiti nakon nekoliko sekundi kako bi se uštedjela energija. |
| **FR-13** | Sustav mora koristiti WiFi modul ESP32 mikrokontrolera za slanje HTTP POST zahtjeva. |
| **FR-14** | Ako je WiFi nedostupan, sustav mora pokušati ponovno pri sljedećem ciklusu mjerenja. |
| **FR-15** | Vrijednosti prikazane na ekranu moraju biti formatirane s pripadajućim jedinicama (°C, %). |
| **FR-16** | Sustav mora prikazati grešku na zaslonu ako nije moguće dohvatiti podatke sa senzora. |

---

## 🔧 Tehnologije

### Hardver

- **Mikrokontroler:** ESP32
- **Senzori:** DHT22 (temperatura/vlažnost), fotootpornik
- **Display:** OLED 128x64 (I2C komunikacija)
- **WiFi Modul:** Ugrađeni WiFi Modul ESP32 mikrokontrolera
- **Dodatno:** Tipkalo

### Softver

- **Biblioteke:** 
  - Adafruit_Sensor
  - DHT.h 
  - Adafruit_SSD1306
  - Wire.h
  - WiFi.h
  - Adafruit_GFX.h
  - HTTPClient.h
  - esp_sleep.h
  - time.h
- **Simulator:** Wokwi za provjeru funkcionalnosti

### Prikaz sustava

![image](https://github.com/user-attachments/assets/71e21156-3d1f-4257-93b1-0a4159c83916)

