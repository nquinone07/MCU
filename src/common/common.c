#include <common.h>
#include <avr/io.h>

// Configure the output direction of a pin
void
set_pin_direction_map(Bank bank, uint8_t pin, Direction direction)
{
    // Check the supplied bank is valid
    if (bank < (sizeof(banks)/sizeof(banks[0])))
    {
        // Initalization direction register 
        volatile uint8_t* direction_register = banks[bank];
        // Based on direction either set or clear the bit
        if (direction == INPUT)
            *direction_register &= ~_BV(pin); 
        else
            *direction_register |= _BV(pin);
    }
}

// Configure the direction of a pin, mapping it to its appropriate major/minor port
void
set_pin_direction_umap(uint8_t pin, Direction direction)
{
    // Setup supplied pin to be an output pin
    const uint8_t const* mapped = map_port(pin);
    if (mapped)
    {
        // Unroll the tupple
        Bank bank           = mapped[0];
        uint8_t mapped_port = mapped[1];
        // Call into set overload
        set_pin_direction_map(bank, mapped_port, direction);
    }
}
