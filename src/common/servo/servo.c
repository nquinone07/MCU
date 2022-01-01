#include <avr/interrupt.h>
#include <avr/io.h>
#include <common.h>
#include <servo.h>

// Servo bank
static servo_t servo_bank[MAX_SERVOS] = {
    [0] = { 
            .pulse_width = 0,
            .in_use      = 0,
            .port_major  = NULL,
            .port_minor  = 0xFF
          },
    [1] = { 
            .pulse_width = 0,
            .in_use      = 0,
            .port_major  = NULL,
            .port_minor  = 0xFF
          },
    [2] = { 
            .pulse_width = 0,
            .in_use      = 0,
            .port_major  = NULL,
            .port_minor  = 0xFF
          },
    [3] = { 
            .pulse_width = 0,
            .in_use      = 0,
            .port_major  = NULL,
            .port_minor  = 0xFF
          }
};
// The next available servo
static uint8_t next_avail = 0;
static int8_t  to_service = 0;

/****************************
 *    INTERRUPT HANDLERS    *
 ****************************/
// Timer 1 output comparison register A match handler
__attribute__((interrupt)) void
TIMER1_COMPA_vect(void)
{
    // Always pulse the previous servo (pulsing it low)
    if ((to_service > 0) && servo_bank[to_service - 1].in_use)
        *(servo_bank[to_service - 1].port_major) ^= _BV(servo_bank[to_service].port_minor);
    // Check if the next servo to service is ge the number of servos allocated
    if (to_service >= next_avail)
    {
        OCR1A = ((unsigned)(TCNT1) + 4) >=  us_to_ticks(REFRESH_INTERVAL, 8) ? TCNT1 + 4 : us_to_ticks(REFRESH_INTERVAL, 8);
        to_service = -1;
    }
    else
    {   
        // Pulse this servo 
        *(servo_bank[to_service].port_major) ^= _BV(servo_bank[to_service].port_minor);
        OCR1A = TCNT1 + servo_bank[to_service].pulse_width;
    }
    ++to_service;
}

/****************************
 *         PUBLIC API       *
 ****************************/
// Configure the timer 
servo_t*
configure_servo(uint8_t pin)
{
    // The servo that shall be returned
    servo_t* acquired = NULL;
    // Null check on servo
    if (next_avail < MAX_SERVOS)
    {
        // Configure servo 
        // Setup supplied pin to be an output pin
        const uint8_t const* mapped = map_port(pin);
        if (mapped)
        {
            // Unroll the tupple
            Bank bank           = mapped[0];
            uint8_t mapped_port = mapped[1];
            // Set the direction of the pin
            set_pin_direction(bank, mapped_port, OUTPUT);
            // Set this servo as in use
            servo_bank[next_avail].in_use = 0x01;
            // Set major/minor port
            servo_bank[next_avail].port_minor = mapped_port;
            servo_bank[next_avail].port_major = &PORTB;
            if (bank != Bank_B)
                servo_bank[next_avail].port_major = bank == Bank_C ? &PORTC : &PORTD;
            // Point at this servo
            acquired = &servo_bank[next_avail++];
            // On first configuration, initialize timer/counter 1
            if (next_avail == 1)
            {
                TCCR1A = 0x00;         // set timer/counter to normal mode
                TCCR1B = _BV(CS11);    // no force output compare, or wgm, set clock select with prescaler of 8
                TCNT1  = 0x00;         // clear the counter
                TIFR1  |= _BV(OCF1A);  // clear any pending interrupts
                TIMSK1 |= _BV(OCIE1A); // enable output compare with OCR2A
            }
        }
    }
    return acquired;
}

// Write servo, given a number of us
void
write_servo_us(servo_t* servo, us_t pulse_micro)
{
    // nullptr check
    if (servo)
    {
        // Convert microseconds to ticks
        if (pulse_micro < PULSE_WIDTH_MIN)
            pulse_micro = PULSE_WIDTH_MIN;
        else if (pulse_micro > PULSE_WIDTH_MAX)
            pulse_micro = PULSE_WIDTH_MIN;
        // Account for pin write delay
        pulse_micro -= PIN_DELAY_US;
        // Convert to ticks
        ticks_t pulse   = us_to_ticks(pulse_micro - PIN_DELAY_US, 8); // prescaler of 8
        ticks_t refresh = us_to_ticks(REFRESH_INTERVAL, 8);           // prescaler of 8
        // Save SREG & disable global interrupts
        uint8_t saved = SREG;
        cli();
        // Initialize servo values
        servo->pulse_width = pulse;
        // Restore
        SREG = saved;
    }
}

// Write the servo given some degree
void
write_servo(servo_t* servo, degree_t degree)
{
    // Pulse width configuration
    us_t pulse_width;

    if (degree < 0)
        degree = 0;
    else if (degree > 180)
        degree = 180;
    // Convert the degree to us_t pulse width
    pulse_width = (uint32_t)(DEGREE_180_DIVIDEND   * (uint32_t)degree)                     + 
                  (uint32_t)((DEGREE_180_REMAINDER * (uint32_t)degree)/DEGREE_180_DIVISOR) +
                  PULSE_WIDTH_MIN;
    // Call into write servo us
    write_servo_us(servo, pulse_width);
}
