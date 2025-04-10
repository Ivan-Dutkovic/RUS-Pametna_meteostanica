/**
 * @file sketch.ino
 * @brief Simulation code for sleep mode operation on an Arduino Mega.
 *
 * This simulation demonstrates sleep mode using a low-power state (power-down) on an Arduino Mega.
 * The simulation blinks an LED, then enters sleep mode where it can be woken by either a button press
 * or a watchdog timer timeout. This code is designed for use with the Wokwi simulation platform.
 */

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

/**
 * @brief Arduino digital pin connected to the LED.
 */
const int LED = 13;

/**
 * @brief Arduino digital pin connected to the button.
 */
const int BUTTON = 2;

/**
 * @brief Flag used to control sleep state.
 *
 * When set to true, the main sleep loop continues.
 * It is cleared by an interrupt (from the watchdog or the button) to exit sleep mode.
 */
volatile bool nothing = true;

/**
 * @brief Flag indicating that the wake-up was triggered by a button interrupt.
 */
volatile bool wakeUpButton = false;

/**
 * @brief Flag indicating that the wake-up was triggered by the watchdog timer.
 */
volatile bool wakeUpTimer = false;

/**
 * @brief Arduino setup function.
 *
 * Configures the LED as an output and the button as an input with an internal pull-up resistor.
 * Attaches an external interrupt to the button (triggered on falling edge) and sets the sleep mode
 * to power-down. Serial communication is started for logging purposes.
 */
void setup() {
    pinMode(LED, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON), wakeUpFromButton, FALLING);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    Serial.begin(9600);
}

/**
 * @brief Arduino main loop function.
 *
 * Blinks the LED a number of times before entering sleep mode.
 * After the sleep mode, the function checks which wake-up flag was set and reports the event via serial.
 */
void loop() {
    // Blink the LED 5 times
    Serial.println("Entering active mode.");
    for (int i = 0; i < 5; i++) {
        digitalWrite(LED, HIGH);
        delay(500);
        digitalWrite(LED, LOW);
        delay(500);
    }
    
    // Ensure LED is off and prepare for sleep mode
    digitalWrite(LED, LOW);
    Serial.println("Entering sleep mode.");
    
    // Reset wake-up flags before entering sleep
    wakeUpButton = false;
    wakeUpTimer = false;
    
    // Enter sleep mode until an interrupt occurs
    enterSleep();
    
    // Upon wake-up, indicate the source of the wake-up
    if (wakeUpButton) {
        Serial.println("Woken up by button interrupt.");
    }
    else if (wakeUpTimer) {
        Serial.println("Woken up by the Watchdog timer.");
    }
}

/**
 * @brief Enters sleep mode.
 *
 * Configures the Arduino to enter the power-down sleep mode.
 * The watchdog timer is set up to allow waking up from sleep after a specified timeout.
 * The function remains in sleep until an interrupt (either from the watchdog or a button press)
 * sets the 'nothing' flag to false.
 */
void enterSleep() {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    // Disable interrupts to ensure a stable setup for sleep
    noInterrupts();
    nothing = true;
    setupWatchdog();  // Configure the Watchdog timer
    sleep_enable();   // Enable sleep mode
    interrupts();     // Re-enable interrupts
    
    // Sleep until an interrupt changes the state of 'nothing'
    while (nothing) {
        sleep_cpu();
    }
    
    sleep_disable();  // Disable sleep mode after waking up
}

/**
 * @brief Sets up the Watchdog Timer.
 *
 * Configures the watchdog timer with an 8-second timeout using the WDP3 and WDP0 bits.
 * The watchdog timer is set in interrupt mode to wake the Arduino from sleep without resetting it.
 * Global interrupts are disabled during configuration to ensure safe setup.
 */
void setupWatchdog() {
    cli();       // Disable global interrupts
    wdt_reset(); // Reset the watchdog timer
    
    // Set watchdog change enable, then configure for 8-second timeout with interrupt enabled
    // Note: The configuration uses WDP3 and WDP0 bits to achieve an 8-second timeout.
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
    
    sei();       // Re-enable global interrupts
}

/**
 * @brief Watchdog Timer Interrupt Service Routine.
 *
 * This ISR is called when the watchdog timer times out.
 * It clears the sleep control flag ('nothing') and sets the wakeUpTimer flag.
 */
ISR(WDT_vect) {
    nothing = false;
    wakeUpTimer = true;
}

/**
 * @brief External Interrupt Service Routine for the button press.
 *
 * This function is attached to the external interrupt associated with the button.
 * When the button is pressed (detected on the falling edge), this function is executed.
 * It clears the sleep control flag ('nothing') and sets the wakeUpButton flag.
 */
void wakeUpFromButton() {
    nothing = false;
    wakeUpButton = true;
}
