#include <avr/io.h>

// Several data registers used to control USART
// SPBRG - Baud rate generator 
// TXSTA - Tx status and control
// RCSTA - Rx status and control
// TXREG - Tx data register
// RXREG - Rx data register 
// PIR1 - Peripheral interrupt flag register
// PIE - Peripheral interrupt enable register

// Procedure:
// -> Map the IO address space into reachable program memory
// -> Enable peripheral interrupt 
// -> Register interrupt service routine to handle raised interrupts from the device 
//      -> Read PIR to know whether data has been Tx/Rx'd
// -> Set baudrate  (115200?)
// -> Select asyncrhonous transmission (otherwise baudrate is useless), e.g. write SYNC bit in
//    TXSTA register (clear this bit) and set BRGH bit to select between high and low speed options
//    for baud rate.
int main(void)
{
}
