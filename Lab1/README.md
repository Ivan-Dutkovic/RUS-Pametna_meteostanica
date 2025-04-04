# Labos 1 - Prekidi
Link na wokwi projekt: https://wokwi.com/projects/427114289094628353

## Sadržaj

1. [Opis zadatka](#opis-projekta)
2. [Komponente](#komponente)
3. [Prikaz sustava](#prikaz-sustava)
4. [Funkcionalni zahtjevi](#funkcionalni-zahtjevi)

## <a name="opis-projekta"></a>1. Opis zadatka

Sustav simulira višestruke prekida s različitim prioritetima

- 3 tipkala
- 3 LED
- HC-SR04 senzor za udaljenost
- Timer koji stvara prekid
- logički analizator
- Arduino Mega mikrokontroler

## <a name="komponente"></a>2. Komponente

| Komponenta         | Količina | Pin |
| ------------------ | -------- | ------------------- |
| Arduino Mega       | 1        | -                   |
| Tipkalo            | 3        | 2, 3, 21            |
| Bijela LED         | 1        | 12                  |
| Narančasta LED     | 1        | 11                  |
| Crvena LED         | 1        | 10                  |
| Plava LED          | 1        | 9                   |
| Zelena LED         | 1        | 8                   |
| HC-SR04            | 1        | TRIG: 6, ECHO: 5    |
| Otpornik 220Ω      | 5        | -                   |
| Logički analizator | 1        | 14 - 18             |

## <a name="prikaz-sustava"></a> 3. Prikaz sustava

![image](https://github.com/user-attachments/assets/e185fe55-ae42-43ea-9b9b-4e7524d2b35a)

## 4. <a name="funkcionalni-zahtjevi"></a>Funkcionalni zahtjevi

| ID    | Opis funkcionalnosti                                                                                 |
| ----- | ---------------------------------------------------------------------------------------------------- |
| FR-1  | Upravljanje LED-icama: Crvena (INT0), Žuta (INT1), Zelena (INT2), Plava (senzor), Bijela (timer).    |
| FR-2  | Detekcija pritiska na tipkala (INT0, INT1, INT2) i generiranje prekida.                              |
| FR-3  | Mjerenje udaljenosti pomoću HC-SR04 senzora i generiranje prekida ako je udaljenost manja od 100 cm. |
| FR-4  | Konfiguracija Timer1 za generiranje prekida svake sekunde (najviši prioritet).                       |
| FR-5  | Obrada prekida prema prioritetima: Timer1 > INT0 > INT1 > INT2 > senzor udaljenosti.                 |
| FR-6  | Vizualizacija aktivnih prekida pomoću odgovarajućih LED-ica.                                         |
| FR-7  | Debounce mehanizam za tipkala kako bi se izbjeglo višestruko okidanje prekida.                       |
| FR-8  | Serial komunikacija za ispisivanje poruka o aktivnim prekidima.                                      |
| FR-9  | Automatsko gašenje svih LED-ica prilikom aktivacije prekida višeg prioriteta.                        |
| FR-10 | Bljeskajuća LED-ica (plava) kao indikacija alarmnog stanja zbog blizine predmeta.                    |
| FR-11 | Testiranje istovremenog okidanja svih prekida i provjera hijerarhije prioriteta.                     |
