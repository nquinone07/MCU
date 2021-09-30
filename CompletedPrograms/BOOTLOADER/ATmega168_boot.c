// Bootloader Registers:
// -- Configure the SPMCSR (Store Program Memory Control and Status Register). This register controls the boot loader
//    operations
//    -> Bit 7: SPM Interrupt Enable, writing 1 to this bit along with writing 1 to the i-bit in the status 
//              register enables the SPM ready interrupt. (Need to have SELFPRGEN bit set as well for interrupt 
//              to be executed)
//    -> Bit 6: RWWSB Read-While-Write Section Busy. When self-programming operation to RWW is initiated, this 
//              bit will be set by hardware. When this bit is set, RWW section cannot be accessed. This is cleared
//              if the RWWSRE (read enable) bit is written to one after a self-programming operation is completed.
//              On page load operations this bit is automatically cleared by hardware
//    -> Bit 5: Reserved
//    -> Bit 4: RWWSRE Read-While-Write Read Enable. When programming (page erase/write) to RWW section, the RWW
//              section is blocked for reading (RWWSB is set by hardware). To re-enable a RWW section software 
//              is required to wait until programming is complete (SELFPRGEN is cleared). If this bit 
//              and SELFPRGEN are both written to one at the same time then the next SPM instruction within 4 clock
//              cycles re-enables the RWW section. Flash loading operations are aborted (and data lost) should this
//              bit be set before the operation completes (indicated by SELFPRGEN written 1).
//    -> Bit 3: BLBSET: Boot Lock Bit Set. 
//    -> Bit 2: PGWRT: Page Write. If this is set and SELFPRGEN is set at the same time then with in the next 4 clock
//              cycles the next SPM instruction will execute a page write of the data stored in the temporary buffer.
//              The page address is taken from the high part of the Z-pointer. The data in R1 and R0 are ignored. This 
//              bit auto-clears upon completion of a page write or if no SPM instruction is executed within the 
//              next 4 clock cycles. CPU is halted during the entire page write operation if NRWW section is addressed.
//    -> Bit 1: PGERS: Page Erase. If this bit and SELFPRGEN are written 1 at the same time, the next SPM instruction
//              within four clock cycles exectues the page erase. Page address is taken from the high part of the
//              Z-pointer. Data in R1/R0 is ignored. This bit auto clears upon completion of a page erase or if no SPM
//              instruction is executed with 4 clock cycles. CPU is halted during the entire page write operation if
//              NRWW section is addressed.
//    -> Bit 0: SELFPRGEN: Self Programming Enable. This bit enables the SPM instruction for the next four clock cycles.
//              written to one together with the RWWSRE, BLBSET, PGWRT, PGERS, the following SPM instruction will have
//              special meaning. If only this bit is written, the following SPM instruction will store the value in
//              R1:R0 in the temporary page buffer addressed by the Z-pointer. LSB of Z-pointer is ignored. This bit
//              auto-clears upon completion of an SPM instruction or if no SPM instruction is executed within 4 clock
//              cycles. During page write/erase operations, SEFLPRGEN bit remains high until the operation is complete.
//    Valid Bit Combinations: 10001, 01001, 00101, 00011, 00001
// Bootloader Steps:
// -> Read data into temporary buffer for program flash 
// -> Load Z-register with page address to write
// -> Indicate page write, writing (10000101) into SPMCSR
// -> Verify write completed and was successful:
//      - wait for SELPRGEN to be cleared
//      - Indicate readback of RWW section -> write 10001001 to SPMCSR 
//      - Verify readback of memory contents
