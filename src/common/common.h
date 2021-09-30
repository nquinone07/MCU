#ifndef COMMON_H
#define COMMON_H

#ifndef F_CPU
#define F_CPU (16000000UL)
#endif
#include <stdint.h>
#include <avr/io.h>

/// Defines ///
// Constants
#define NULL          ((void *)0)
#define PIN_DELAY_US  (2)
#define US_PER_SEC    (1000000UL)
#define CYCLES_PER_US ((F_CPU) / (US_PER_SEC))
// No-op
#define noop ((void) 0)
// Pin mapping from arduino to atmel128
#define map_port(pin) \
    ( ((pin) >= (sizeof(pin_map)/sizeof(pin_map[0])))\
        ? (NULL)\
        : (pin_map[pin]) ) 
/// Function Overloading ///
#define set_pin_direction(_1, ...) _Generic((_1),                            \
                                             uint8_t: set_pin_direction_umap, \
                                             default: set_pin_direction_map)(_1, __VA_ARGS__)
/// Typedefs ///
typedef uint8_t  tuple_t[2];

/// Enumerations ///
// Various ports (banks)
typedef enum
{
    Bank_B = 0,
    Bank_C,
    Bank_D
} Bank;

// Pin direction (no alternate function setting yet)
typedef enum
{   
    INPUT = 0,
    OUTPUT
} Direction;

/// Maps ///
// All data direction banks
static volatile uint8_t* banks[] = {
        &DDRB, &DDRC, &DDRD
};

// Pin mapping, each pin (unmapped) index to its major/minor tuple
static const tuple_t pin_map[] = 
{
    /* Digital Pin Mapping */
    {Bank_D, PORTD0}, {Bank_D, PORTD1}, {Bank_D, PORTD2},
    {Bank_D, PORTD3}, {Bank_D, PORTD4}, {Bank_D, PORTD5},
    {Bank_D, PORTD6}, {Bank_D, PORTD7}, {Bank_B, PORTB0},
    {Bank_B, PORTB1}, {Bank_B, PORTB2}, {Bank_B, PORTB3},
    {Bank_B, PORTB4}, {Bank_B, PORTB5}, 
    /* Analog Pin Mapping - ?*/
    {Bank_C, PORTC0}, {Bank_C, PORTC1}, {Bank_C, PORTC2}, 
    {Bank_C, PORTC3}, {Bank_C, PORTC4}, {Bank_C, PORTC5} 
};

/// Functions ///
// Configure the direction of a pin
void                              // OUT: NONE
set_pin_direction_map(Bank,       // IN : Bank of pin to set
                      uint8_t,    // IN : Port to set
                      Direction); // IN : Direction of pin
// Configure the direction of a pin, mapping it to its appropriate major/minor port
void                                // OUT: NONE 
set_pin_direction_umap(uint8_t,    // IN : pin (unmapped)
                       Direction); // IN : direction (input or output)
#endif 
