#ifndef F_CPU
#define F_CPU 20000000UL
#endif

#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "macros.h"

#define noop ((void)0)


// How the LED should be manipulated
typedef enum FLASH
{
    ON,
    OFF
} FLASH;

// Simply flash an LED given by the pin
void
led_flash(uint8_t digital_pin, FLASH direction)
{   
    // Configure the port b 5 to be an output pin
    DDRB  |= _BV(digital_pin);
    // No op for synchronization 
    noop;
    // Simple, turn it on 
    if (direction == ON)
        PORTB |= _BV(digital_pin);
    else 
        PORTB &= ~_BV(digital_pin);
}

// Interrupt Registration
__attribute__ ((interrupt)) void 
PCINT0_vect(void) 
{
    // Pin we have selected
    static uint8_t pin_b = 0;
    // LED some time to stay alive
	_delay_ms(50);
    // Turn off the previous LED
    led_flash(pin_b, OFF);
	// PORTB pins will be selected from 0 to 5
    pinb = (pinb + 1) % 6;
	// Turn on the next one 
	led_flash(pin_b, ON);
}

// Pretty flashing lights!!
void 
flashing_lights()
{
    // NOTE: The process of flashing: The process of flashing lights is interrupt driven. Lighting 
    //       an LED by asserting a pin, will generate another interrupt after the ISR has exited.
    // Kick off first pin light, the rest will be driven by interrupts
    led_flash(pin_b, ON);
}

// Main
int 
main(void)
{
    // Global Interrupt Enable
    SREG |= SHIFT_ONE_BY(7);
    // Unmask the Pin Change Interrupt for PCINT5..0
    PCMSK0 |= _BV(DDB0) | _BV(DDB1) | _BV(DDB2) | _BV(DDB3) | _BV(DDB4) | _BV(DDB5); 
    // Enable Pin Change Interrupts for PCINT7..0 (Only Port B pins are unmasked)
    PCICR |= _BV(PCIE0);
    // Call into flashing lights, Interrupt driven procedure
    flashing_lights();
    return 0;
}
