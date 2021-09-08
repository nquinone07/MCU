#ifndef F_CPU
#define F_CPU 20000000UL
#endif

#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define noop ((void)0)


// How the LED should be manipulated
typedef enum FLASH
{
    ON,
    OFF
} FLASH;

// Simply flash an LED given by the pin
void
led_flash_b(uint8_t digital_pin, FLASH direction)
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


// TODO: SPDT toggled left/right/middle. If left, the "left" 3 LEDs should blink
//       in a mustang turn signal fashion. Symmetrically, right toggle should blink
//       the "right" 3 LEDs. Middle, indicates a

// Interrupt Registration
__attribute__ ((interrupt)) void 
PCINT2_vect(void) 
{
    // TODO: PCINT2 (Pin Change Interrupt 2) is the ISR controlling interrupt handling
    //       for pin changes PCINT23..16
}

// Pretty flashing lights!!
void 
mustang_lights()
{
    // Set the sleep mode to IDLE allowing pin change interrupts to wake us up
    set_sleep_mode(SLEEP_MODE_IDLE);
    // Enable sleep
    sleep_enable();
    while (1)
    {
        // Sleep the CPU, pin change interrupts will wake us up
        sleep_cpu();
    }
    sleep_disable();
}

// Main
int 
main(void)
{
    /* Pin Configuration */
    // PORTD Pins 6/7 are configured as input pins (set 0 in data direction register)
    DDRD &= ~_BV(PORTD7) & ~_BV(PORTD6);
    noop; // Probably not necessary since we are not writing to PORTD pins, but nontheless noop for sync
    
    // Global Interrupt Enable
    SREG |= _BV(7); // setting I-Bit
    
    /* Pin Change Interrupt Enabling */
    // Unmask the Pin Change Interrupt for PCINT23/22
    PCMSK2 |= _BV(DDD7) | _BV(DDD6);
    // Enable Pin Change Interrupts for PCINT23..16 (these interrupts map to PORT D)
    PCICR |= _BV(PCIE2);


    // Set timer control register -- setting clock source to clkio / 1024 (from prescaler)
    TCCR0A = _BV(WGM00) | _BV(WGM01) | 
    TCCR0B = _BV(CS02) | _BV(CS00) | _BV(WGM02);
    // Unmask interrupts for TOV0 (timer 0 overflow)
    TOIE0 = _BV(TOIE0);
    // Atomic write to 0CR0A register, setting the TOP value for TCNT0
    // Disable the Global interrupt flag in SREG (This is the reverse process shown above e.g. SREG &= ~_BV(7);
    // Here we choose to use avr instruction cli (clear global interrupt flag)
    uint8_t saved_sreg = SREG;
    // On critical section enter
    __asm__ __volatile__ ("cli");
    OCR0A = 0xBB; // arbitrary
    // On critical section exit    
    SREG = saved_sreg;
    
    // Call into flashing lights
    mustang_lights();
    return 0;
}
