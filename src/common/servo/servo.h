#ifndef SERVO_H
#define SERVO_H
#include <stdint.h>

/// Defines ///
#define PULSE_WIDTH_MAX      (2500)  // Maximum pulse width for the servo
#define PULSE_WIDTH_MIN      (500)   // Minimum pulse width for the servo
#define REFRESH_INTERVAL     (20000) // Refresh rate at which to pulse the servo
#define DEGREE_180_DIVISOR   (180)   // Divisor of 180, used for conversion from us to degrees
#define DEGREE_180_DIVIDEND  (((PULSE_WIDTH_MAX) - (PULSE_WIDTH_MIN))/(DEGREE_180_DIVISOR)) // Dividend used for converting pulse width to degrees
#define DEGREE_180_REMAINDER (((PULSE_WIDTH_MAX) - (PULSE_WIDTH_MIN))%(DEGREE_180_DIVISOR)) // Remainder used for converting pulse width to degrees 
#define us_to_ticks(_us, prescaler) ((ticks_t)(((us_t)(CYCLES_PER_US) * (_us))/((us_t)(prescaler)))) // Conversion from us to ticks, given us and clock prescaler
#define MAX_SERVOS 4 // Defining this to be 4, for now 

/// Typedefs ///
typedef uint32_t ticks_t;
typedef uint32_t us_t;
typedef uint8_t  percent_t;
typedef uint8_t  degree_t;

/// Structures ///
// Timer structure
typedef struct
{
    // The width of the pulse
    ticks_t pulse_width; 
    // Flagging indicating if this servo is taken
    uint8_t in_use: 1;
    // Major port value (PORTB/C/D) 
    volatile uint8_t* port_major;
    // Minor port value (e.g. PORTB/1/2/3/4/5/6/7)
    uint8_t port_minor;
} servo_t;

/// Functions ///
// Configure the servo given a pin
servo_t*                   // OUT: Servo that was acquired and attacehd to the requested pin
configure_servo(uint8_t);  // IN : a pin that is attached to the control line of a servo
// Write the servo given some degree
void                   // OUT: NONE                 
write_servo(servo_t*,  // IN : the servo to move
            degree_t); // IN : degrees by which to move the servo
// Write the servo given microseconds
void                     // OUT: NONE
write_servo_us(servo_t*, // IN : servo to move using us pulse width
               us_t);    // IN : number of us to move the servo by
#endif 
