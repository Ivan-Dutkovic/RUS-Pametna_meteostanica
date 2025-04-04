#include <avr/interrupt.h>

#define BUTTON0      2
#define BUTTON1      3
#define BUTTON2      21
#define LED_INT0     8
#define LED_INT1     9
#define LED_INT2     10
#define LED_Sensor   11
#define LED_Timer    12
#define TRIG_PIN     6
#define ECHO_PIN     5

#define TIMER1_PRESCALER   1024
#define TIMER1_COMPARE     15624
#define BLINK_INTERVAL     200
#define DEBOUNCE_DELAY     50
#define TIMER_DELAY        1000
#define ALARM_DISTANCE     100

#define LOGIC_ANALYZER_PIN0 14 
#define LOGIC_ANALYZER_PIN1 15 
#define LOGIC_ANALYZER_PIN2 16 
#define LOGIC_ANALYZER_TIMER 17
#define LOGIC_ANALYZER_SENSOR 18 

volatile bool     intFlag[3] = {0};
volatile unsigned long lastInterruptTime[3] = {0}, 
                       lastTimerTime = 0;
volatile bool     distanceAlert = 0,
                  timerFlag = 0;

void setup(){
    pinMode(LED_INT0,    OUTPUT);
    pinMode(LED_INT1,    OUTPUT);
    pinMode(LED_INT2,    OUTPUT);
    pinMode(LED_Sensor,  OUTPUT);
    pinMode(LED_Timer,   OUTPUT);

    pinMode(BUTTON0, INPUT_PULLUP);
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    pinMode(LOGIC_ANALYZER_PIN0, OUTPUT);
    pinMode(LOGIC_ANALYZER_PIN1, OUTPUT);
    pinMode(LOGIC_ANALYZER_PIN2, OUTPUT);
    pinMode(LOGIC_ANALYZER_TIMER, OUTPUT);
    pinMode(LOGIC_ANALYZER_SENSOR, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(BUTTON0), ISR_INT0, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON1), ISR_INT1, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON2), ISR_INT2, FALLING);

    TCCR1A = 0;
    TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);
    OCR1A  = TIMER1_COMPARE;
    TIMSK1 = (1<<OCIE1A);
    
    Serial.begin(9600);
    sei();
}

void loop(){
    float d = measureDistance();
    distanceAlert = (d > 0 && d < ALARM_DISTANCE);

    if(timerFlag){ handleTimerInterrupt(); }
    
    handleBlinking();
    
    triggerDistanceAlert();
}

ISR(TIMER1_COMPA_vect){
  digitalWrite(LOGIC_ANALYZER_TIMER, HIGH);
    timerFlag = true;
    lastTimerTime = millis();
    digitalWrite(LOGIC_ANALYZER_TIMER, LOW);
}

void handleTimerInterrupt(){
    digitalWrite(LED_Timer, HIGH);
    Serial.println("TIMER INTERRUPT (HIGHEST PRIORITY)");
    delay(200);
    digitalWrite(LED_Timer, LOW);
    timerFlag = false;
}

void ISR_INT0(){
  digitalWrite(LOGIC_ANALYZER_PIN0, HIGH);
  handleInterrupt(0, LED_INT0, "INT0 (HIGH priority) triggered");
  digitalWrite(LOGIC_ANALYZER_PIN0, LOW);
}
void ISR_INT1(){ 
  digitalWrite(LOGIC_ANALYZER_PIN1, HIGH);
  handleInterrupt(1, LED_INT1, "INT1 (MEDIUM priority) triggered");
  digitalWrite(LOGIC_ANALYZER_PIN1, LOW);
}
void ISR_INT2(){
  digitalWrite(LOGIC_ANALYZER_PIN2, HIGH);
  handleInterrupt(2, LED_INT2, "INT2 (LOW priority) triggered");
  digitalWrite(LOGIC_ANALYZER_PIN2, HIGH);
}

void handleInterrupt(int i, int led, const char* msg){
    if((millis() - lastInterruptTime[i]) < DEBOUNCE_DELAY) return;
    lastInterruptTime[i] = millis();
    intFlag[i] = true;
    Serial.println(msg);
}

void handleBlinking(){
    for(int i=0; i<3; i++){
        if(intFlag[i]){
            blinkLed((i==0) ? LED_INT0 : (i==1) ? LED_INT1 : LED_INT2);
            intFlag[i] = false;
        }
    }
}

void blinkLed(int led){
    unsigned long start = millis();
    while(millis() - start < 1000){
        digitalWrite(led, !digitalRead(led));
        delay(BLINK_INTERVAL);
    }
    digitalWrite(led, LOW);
}

float measureDistance(){
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long t = pulseIn(ECHO_PIN, HIGH, 30000);
    return (t > 0) ? t / 58.0 : -1;
}

void triggerDistanceAlert() {
    static unsigned long prev = 0;
    if (distanceAlert) {
      digitalWrite(LOGIC_ANALYZER_SENSOR, HIGH);
        if (millis() - prev >= 200) {
            prev = millis();
            digitalWrite(LED_Sensor, !digitalRead(LED_Sensor));
        }
        Serial.println("ALARM: Predmet preblizu (<100cm, LOWEST PRIORITY)!");
        digitalWrite(LOGIC_ANALYZER_SENSOR, LOW);
    } else {
        digitalWrite(LED_Sensor, LOW);  // Turn off LED when distance is safe
    }
}