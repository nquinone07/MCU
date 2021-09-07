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
// Currently selected pin
uint8_t pin_b = 5;

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
    // Disable interrupts while inside of the ISR
    //PCICR &= ~_BV(PCIE0);
    // Turn off the previous LED
    led_flash(pin_b, OFF);
    // Decrement pin_b, wrapping around to 5
    if (!pin_b)
        pin_b = 5;
    else
        --pin_b;
}

// Pretty flashing lights!!
void 
flashing_lights()
{
    // Set the sleep mode, note IDLE is chosen here as according to the docs
    // Pin change interrupts will wake us up
    set_sleep_mode(SLEEP_MODE_IDLE);
    // Enable sleep
    sleep_enable();
    // Kick off first pin light, the rest will be driven by interrupts
    led_flash(pin_b, ON);
    while (1)
    {
        // Sleep the CPU, pin change interrupts will wake us up
        sleep_cpu();
        // Waking up requires that an interrupt was had.. Give a delay before sleeping to allow
        // LED some time to stay alive
        _delay_ms(50);
        // Reenable interrupts
        PCICR |= _BV(PCIE0);
        // Turn on the next one 
        led_flash(pin_b, ON);
    }
    sleep_disable();
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
    // Call into flashing lights
    flashing_lights();
    return 0;
}
