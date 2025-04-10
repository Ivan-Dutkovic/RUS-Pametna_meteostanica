# Labos 2 - Upravljanje potrošnjom energije
Link na wokwi projekt: https://wokwi.com/projects/427696291457591297

## Opis zadatka

Cilj zadatka je omogućiti da mikrokontroler prelazi u nisku potrošnju energije (sleep mode) nakon izvršavanja određenih radnji (bljeskanje LED-om) te se zatim probudi pomoću vanjskog prekida (tipka) ili watchdog timera. Kroz ovaj projekt, korisnici mogu naučiti kako koristiti sleep modove kako bi smanjili potrošnju energije u aplikacijama koje zahtijevaju dugotrajno funkcioniranje na baterije.

## Hardverske komponente

- **Arduino Mega**: Glavna razvojna ploča koja pokreće program.
- **LED dioda**: Spojena na digitalni pin 13, koristi se za indikaciju rada sustava.
- **Tipka**: Spojena na digitalni pin 2 s internim pull-up otpornikom, omogućava vanjski prekid za buđenje uređaja.

## Prikaz sustava
![image](https://github.com/user-attachments/assets/413ea662-4c6e-4626-8407-e425f6224644)

## Opis rješenja

Program se sastoji od glavne petlje gdje se LED dioda bljeska nekoliko puta, nakon čega se sustav priprema za sleep mod. Prije ulaska u sleep mod, resetiraju se zastavice koje označavaju način buđenja.  
- **Sleep mod**: Arduino prelazi u `SLEEP_MODE_PWR_DOWN`, što je najniži način rada s minimalnom potrošnjom energije.
- **Buđenje**: Dva mehanizma omogućuju buđenje uređaja:
  - **Prekid tipke**: Ako korisnik pritisne tipku, vanjski prekid se aktivira i uređaj se budi.
  - **Watchdog timer**: Ako nije pritisnuta tipka, watchdog timer (konfiguriran na 8 sekundi) aktivira prekid i buđenje se dogodi automatski.

## Sleep modovi mikrokontrolera: Usporedba i primjena

Mikrokontroleri poput onih na Arduino platformi podržavaju više sleep modova, a u ovom projektu se koristi **power-down** mod:
- **Power-down mode**:  
  - Prednosti: Minimalna potrošnja energije, idealno za aplikacije koje zahtijevaju dugotrajnu radnu autonomiju.
  - Nedostaci: Većina periferija je isključena, što može rezultirati sporijim buđenjem.
- **Idle mode**:  
  - Prednosti: Neke periferije ostaju aktivne, što omogućuje brže odzivanje.
  - Nedostaci: Veća potrošnja energije u odnosu na power-down mode.
- **Power-save mode**:  
  - Prednosti: Omogućava rad određenih perifernih jedinica (npr. Timer) uz smanjenu potrošnju energije.
  - Nedostaci: Konzumira više energije nego power-down mod, ali manje nego u idle modu.
- **Standby i Extended Standby**:  
  - Prednosti: Brže vrijeme buđenja uz određenu uštedu energije.
  - Nedostaci: Komplikiranija implementacija i nešto veća potrošnja energije nego kod power-save moda.

Iako konkretna realna mjerenja nisu uključena u ovaj projekt, općenito se zna da:
- **Power-down mode** ima vrlo nisku potrošnju, često ispod 1 mA, što je idealno za aplikacije temeljene na baterijama.
- **Idle mode** pokazuje veću potrošnju zbog aktivnih perifernih jedinica, što može biti nekoliko puta veće od power-down moda.
- **Power-save mode** nudi kompromis između funkcionalnosti i potrošnje, omogućujući rad određenih modula uz nešto veću potrošnju u odnosu na power-down mode.

## Savjeti za optimizaciju energetske učinkovitosti

- **Korištenje najnižeg mogućeg sleep moda**: Kad god je to moguće, prebacite se u power-down mode kako biste smanjili potrošnju energije.
- **Isključivanje nepotrebnih periferija**: Prije ulaska u sleep mod, onemogućite sve vanjske i unutarnje module koji nisu nužni.
- **Pametno buđenje**: Koristite vanjske prekide ili watchdog timer umjesto periodičnog buđenja kroz petlje, kako biste minimizirali vrijeme aktivnog rada.
- **Optimizacija koda**: Izbjegavajte nepotrebne operacije unutar glavnih petlji te koristite efikasne algoritme.
- **Realna mjerenja**: Redovito mjerite potrošnju struje u stvarnom okruženju kako biste identificirali i eliminirali nepotrebno trošenje energije.
