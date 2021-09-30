#ifndef F_CPU
#define F_CPU 20000000UL
#endif

#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define noop ((void)0)
#define FALSE 0
#define TRUE !FALSE

// How the LED should be manipulated
typedef enum FLASH
{
    ON,
    OFF
} FLASH;

// PORTB pins connected
const uint8_t leds [PORTB5 - PORTB0 + 1] = 
{
    PORTB5, PORTB4, PORTB3, PORTB2, PORTB1, PORTB0
};

// Pin index at which starting ON'ing LEDs
uint8_t pin_start_idx = 0;

// Number of interrupts that have elapsed
uint8_t overflows = 0; // 

// The max number of timer overflows before an LED can be turned on
static const uint8_t MAX_OVF = 10; // The calculation to give an appropriate estimation of what this should be is:
                                   // -> 1/(clk_freq/(prescalar * constant)) * overflow_cnt

// Disable timer
void 
timer_disable()
{
    // Mask interrupts for TOV0 (timer 0 overflow)
    TIMSK0 &= ~_BV(TOIE0);
}

// Disable timer
void 
timer_reset()
{
    // Mask interrupts for TOV0 (timer 0 overflow)
    TIMSK0 |= _BV(TOIE0);

    // Disable the Global interrupt flag in SREG (This is the reverse process shown above e.g. SREG &= ~_BV(7))
    // Here we choose to use avr instruction cli (clear global interrupt flag)
    uint8_t saved_sreg = SREG;

    // On critical section enter
    __asm__ __volatile__ ("cli");

    /* CRITICAL SECTION */
    // Reset the timer count value 
    TCNT0  = 0;
    /* CRITICAL SECTION */

    // On critical section exit 
    SREG = saved_sreg;

    // Reset the timer/counter interrupt flag
    TIFR0 &= ~_BV(TOV0);
}

// Timer configuration 
void 
timer_configure()
{
    // Disable the Global interrupt flag in SREG (This is the reverse process shown above e.g. SREG &= ~_BV(7))
    // Here we choose to use avr instruction cli (clear global interrupt flag)
    uint8_t saved_sreg = SREG;

    // On critical section enter
    __asm__ __volatile__ ("cli");
   
    /** CRITICAL SECTION **/
    // Set Timer Control Register to normal mode
    TCCR0A = 0;
    TCCR0B = _BV(CS00) | _BV(CS02);
    // Unmask interrupts for TOV0 (timer 0 overflow)
    TIMSK0 |= _BV(TOIE0);
    // Reset Timer/Counter
    TCNT0 = 0;
    /** CRITICAL SECTION **/
    
    // On critical section exit    
    SREG = saved_sreg;
}

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

// Turn on/off lights
void 
toggle_leds(uint8_t start, uint8_t end, FLASH direction)
{
    // Turn all lights
    if ((start < end) && (end <= sizeof(leds)))
    {
        for (uint8_t i = start; i < end; ++i)
        {
            led_flash_b(leds[i], direction);
        }
    }
}
// Timer Overflow 
__attribute__ ((interrupt)) void 
TIMER0_OVF_vect(void) 
{
    // Check overflows 
    overflows = (overflows + 1) % MAX_OVF;
    if (!overflows)
    {
        if (!(pin_start_idx % 3))
            toggle_leds(pin_start_idx, pin_start_idx + 3, OFF);
        led_flash_b(leds[pin_start_idx], ON);
        ++pin_start_idx;
        if (!(pin_start_idx % 3))
            pin_start_idx -= 3;
    }
}

// Pin Change Interrupt 2 Registration
__attribute__ ((interrupt)) void 
PCINT2_vect(void) 
{
    // Timer configuration happens on the first interrupt handle for switch throw
    static uint8_t just_once = FALSE;
    if (!just_once)
    {
        timer_configure();
        just_once = TRUE;
    }
    // Disable timer 
    timer_disable();
    
    // Reset overflows
    overflows = 0;

    // Toggle all LEDs on off 
    toggle_leds(0, sizeof(leds), OFF);

    // Reset the pin index
    if (pin_start_idx < 3)
    {
        pin_start_idx = 3;
    }
    else
    {
        pin_start_idx = 0;
    }
    // Timer reset on exit
    timer_reset();
}

// Mustang lights 
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
    DDRD &= ~_BV(DDD7) & ~_BV(DDD6);
    noop; // Probably not necessary since we are not writing to PORTD pins, but nontheless noop for sync
    
    // Global Interrupt Enable
    SREG |= _BV(7); // setting I-Bit
    
    /* Pin Change Interrupt Enabling */
    // Unmask the Pin Change Interrupt for PCINT23/22
    PCMSK2 |= _BV(PCINT23) | _BV(PCINT22);
    // Enable Pin Change Interrupts for PCINT23..16 (these interrupts map to PORT D)
    PCICR |= _BV(PCIE2);
    // Call into mustang lights
    mustang_lights();

    return 0;
}
