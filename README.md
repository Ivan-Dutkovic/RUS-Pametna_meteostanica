# Pametna meteostanica
https://wokwi.com/projects/426855239853016065
## 📌 Opis projekta

Projekt **pametna meteostanica** omogućuje praćenje vremenskih uvjeta u realnom vremenu kroz kombinaciju senzora i IoT funkcionalnosti.  
**Cilj:** Pružiti precizno mjerenje okoline, energetski efikasno rješenje s mogućnošću daljinskog nadzora.

### Ključne značajke:

- 🌡️ Mjerenje temperature i vlažnosti zraka (DHT22 senzor)
- ☀️ Detekcija intenziteta svjetlosti (fotootpornik)
- 📺 Prikaz podataka na OLED zaslonu
- 💤 Energetski efikasni način rada (duboki san)
- 🔘 Interakcija preko tipke za budenje sustava
- 📲 Automatske notifikacije na mobilnu aplikaciju

---

## 🛠️ Funkcijski zahtjevi

### Senzori i komponente

| Komponenta       | Opis |
| ---------------- | ---- |
| DHT22            | Mjeri temperaturu i vlažnost zraka |
| OLED zaslon (128x64) | Prikazuje podatke u realnom vremenu |
| Fotootpornik     | Mjeri intenzitet ambientalog svjetla |
| Tipka            | Ručno budenje iz dubokog sna |

### Funkcionalnosti

| Funkcija | Opis |
| -------- | ---- |
| Prikaz podataka na ekranu | Prikaz trenutnih podataka o vremenu na OLED ekranu |
| Energetski štedljiv način rada | ESP32 prelazi u duboki san između mjerenja |
| Ručna kontrola | Mogućnost buđenja tipkom za trenutni prikaz |
| Mobile Notifications | HTTP POST zahtjevi |

---

## 🔧 Tehnologije

### Hardver
- **Mikrokontroler:** ESP32
- **Senzori:** DHT22 (temperatura/vlažnost), fotootpornik
- **Display:** OLED 128x64 (I2C komunikacija)
- **WiFi Modul:** Ugrađeni WiFi Modul ESP32 mikrokontrolera
- **Dodatno:** Tipka, otpornici

### Softver
- **Razvojno okruženje:** Arduino IDE
- **Biblioteke:** 
  - Adafruit_Sensor (DHT22)
  - DHT.h 
  - Adafruit_SSD1306 (OLED)
- **Simulator:** Wokwi za provjeru funkcionalnosti

---

## ⚙️ Uporaba

1. **Inicijalizacija sustava**  
   - Zaslon prikazuje početni ekran s naslovom projekta
2. **Automatsko mjerenje**  
   - Svakih 30 minuta se mjere novi podaci i šalju korisniku kao notifikacija
3. **Energija**  
   - Nakon prikaza podataka sustav prelazi duboki san
4. **Ručna kontrola**  
   - Pritisak tipke budi sustav za trenutni prikaz, a dugi pritisak šalje ponovnu obavijest
