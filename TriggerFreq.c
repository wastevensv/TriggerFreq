/* TriggerFreq - William A Stevens V - 4/25/15
 * Uses interrupts to count the pulses within a period of time to determine frequency.
 * Designed to run a single H-Bridge off of a mono audio signal.
 *
 * When the frequency on SIG_IN is near 1300 Hz, ENA_OUT is set high, and DIR_OUT is set low.
 * When the Frequency on SIG_IN is near 2600 Hz, ENA_OUT is set high, and DIR_OUT is set high.
 */

// --- Prerequisites ---
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "macros.h"

// --- Settings ---
#define SAMP_RATE 4 // Samples per second (ex: 4 samples per second = 250ms sample length)
#define ERR_MARGIN 50 // Error margin in Hz
#define FREQ_A 1300 // Forward frequency in Hz
#define FREQ_B 2600 // Reverse frequency in Hz

// --- Pin Definitions ---
#define ENA_OUT PB3
#define DIR_OUT PB4

volatile uint16_t pulse_count;

// --- Interrupt function definition ---
ISR(INT0_vect) { // Runs every time pin 6 detects a rising edge
  pulse_count++;
}

int main(void) {
  // --- Initialize ---
  clock_prescale_set(clock_div_1);
  DDRB = ((1 << ENA_OUT)|(1 << DIR_OUT));
  
  // --- Setup Interrupt 0 ---
  GIMSK |= (1 << INT0); // Enable INT0
  MCUCR |= ((1 << ISC00)|(1 << ISC01)); // Trigger INT0 on rising edge.
  
  while(1) {

    pulse_count = 0;
    sei();
    _delay_ms(1000/SAMP_RATE);
    cli();

    if(((pulse_count + ERR_MARGIN) > (FREQ_A/SAMP_RATE)) && ((pulse_count - ERR_MARGIN) < (FREQ_A/SAMP_RATE))) { // If pulse_count is near FREQ_A (+/- ERR_MARGIN)...
      clear_bit(PORTB, DIR_OUT); // Set DIR_OUT to low (forward)
      set_bit(PORTB, ENA_OUT); // Set ENA_OUT to high (enabled)
    } else if(((pulse_count + ERR_MARGIN) >  (FREQ_B/SAMP_RATE)) && ((pulse_count - ERR_MARGIN) < (FREQ_B/SAMP_RATE))) {// If pulse_count is near FREQ_B (+/- ERR_MARGIN)...
      set_bit(PORTB, DIR_OUT); // Set DIR_OUT to high (reverse)
      set_bit(PORTB, ENA_OUT); // Set ENA_OUT to high (enabled)
    } else {
      clear_bit(PORTB, ENA_OUT); // Set ENA_OUT to low (disabled)
    }
  }
}
