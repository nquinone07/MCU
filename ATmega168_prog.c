#ifndef F_CPU
#define F_CPU 20000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define noop ((void)0)
int main(void)
{
    // Configure the port b 5 to be an output pin
    DDRB  |= 0 << PORTB5;
    // No op for synchronization 
    noop;
    // Simple, turn it on and off
    while (1)
    {
        _delay_ms(500);
        PORTB |= 1 << 5;
        _delay_ms(500);
        PORTB |= 0 << 5;
    }
    return 0;
}
