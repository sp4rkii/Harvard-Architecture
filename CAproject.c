#include <stdio.h>


int pc = 0;
int instructionMemory[1024];
short int DataMemory[2048];
short int GPRS[64];
short int SREG;

// int Carry_Flag (short int value1, short int value2) {
//     int temp1 = value1 & 0x000000FF = 0b00000000000000000000000011111011;
//     int temp2 = value2 & 0x000000FF = 0b00000000000000000000000000000101;
//     int mask=0b00000000000000000000000100000000;
//     if( ((temp1 OP temp2) & mask) == MASK) {
//         return 1;
//     } else {
//         return 0;
//     }
// }


