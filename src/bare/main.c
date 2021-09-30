#include <includes.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdint.h>

/// Globals ///
servo_t servo;

// Main
int 
main(void)
{
    // Enable interrupts
    SREG |= _BV(7);
    /* Configure servo */
    servo_t* servo_1 = configure_servo(11);
    /* Turn the servo */
    int pos;
    for (pos = 0; pos <= 180; pos += 1)
    {
        write_servo(servo_1, pos);
        _delay_ms(50);
    }
    for (pos = 180; pos >= 0; pos += 1)
    {
        write_servo(servo_1, pos);
        _delay_ms(15);
    }
    return 0;
}
