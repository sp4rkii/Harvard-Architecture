#include <stdio.h>
#include <stdint.h>

int pc = 0;
short int instructionMemory[1024];
int8_t DataMemory[2048];
int8_t GPRS[64];
int8_t SREG;
void decode(short int);
void execute(int,int8_t,int8_t,int,int);
void Fetch(){
    short int instruction=0;
    for(pc;pc<1024;pc++){
        instruction=instructionMemory[pc];
        decode(instruction);
    }
}
void decode(short int instruction){
    int opcode=((instruction& 0x000000FF) & 0b00000000000000001111000000000000)>>12;
    int R1=((instruction& 0x000000FF) & 0b000000000000000000001111111110000000)>>6;
    int R2=((instruction& 0x000000FF) & 0b000000000000000000000000000111111111);
    int immediate=((instruction& 0x000000FF) & 0b000000000000000000000000000111111111);
    int8_t value1=GPRS[R1];
    int8_t value2=GPRS[R2];
    execute(opcode,value1,value2,immediate,R1);
}
void execute(int opcode,int8_t value1,int8_t value2,int immediate,int R1){

    int8_t result;
    SREG = 0b00000000;

    int carryFlag = 0;
    int overflowFlag = 0;
    int negativeFlag = 0;
    int signFlag = 0;
    int zeroFlag = 0;

    switch(opcode){
        case 0: // ADD Operation
            // Perform addition
            result = value1 + value2;
            // Update flags
            // Carry Flag
            int8_t carryMask = 0x100; // Mask to isolate the 9th bit
            int8_t temp1 = value1 & 0xFF; // Extract 8-bit value of value1
            int8_t temp2 = value2 & 0xFF; // Extract 8-bit value of value2
            carryFlag = (((temp1 + temp2) & carryMask) == carryMask) ? 1 : 0;
            if(carryFlag)
                SREG = SREG | 0b10000000 ;
            // Overflow Flag
            overflowFlag = ((value1 > 0 && value2 > 0 && result < 0) || (value1 < 0 && value2 < 0 && result > 0));
            if(overflowFlag)
                SREG = SREG | 0b01000000 ;
            // Negative Flag
            negativeFlag = (result < 0);
            if(negativeFlag)
                SREG = SREG | 0b00100000 ;
            // Sign Flag
            signFlag = ((negativeFlag ^ overflowFlag) != 0);
            if(signFlag)
                SREG = SREG | 0b00010000 ;
            // Zero Flag
            zeroFlag = (result == 0);
            if(zeroFlag)
                SREG = SREG | 0b00001000 ;
            break;
        case 1: // Subtract Operation
            // Perform subtraction
            result = value1 - value2;
            // Update flags
            // Overflow Flag
            overflowFlag = ((value1 < 0 && value2 > 0 && result > 0) || (value1 > 0 && value2 < 0 && result < 0));
            if(overflowFlag)
                SREG = SREG | 0b01000000 ;
            // Negative Flag
            negativeFlag = (result < 0);
            if(negativeFlag)
                SREG = SREG | 0b00100000 ;
            // Sign Flag
            signFlag = ((negativeFlag ^ overflowFlag) != 0);
            if(signFlag)
                SREG = SREG | 0b00010000 ;
            // Zero Flag
            zeroFlag = (result == 0);
            if(zeroFlag)
                SREG = SREG | 0b00001000 ;
            break;
        case 2: // Multiply Operation
            // Perform multiplication
            result = value1 * value2;
            // Update flags
            // Negative Flag
            negativeFlag = (result < 0);
            if(negativeFlag)
                SREG = SREG | 0b00100000 ;
            // Zero Flag
            zeroFlag = (result == 0);
            if(zeroFlag)
                SREG = SREG | 0b00001000 ;
            break;
        case 3: // Load Immediate Operation
            // Load immediate value into register
            GPRS[R1] = immediate;
            // No flags are affected
            break;
        case 4: // Branch If Equal Zero Operation
            if (value1 == 0) {
                // Update PC to immediate value
                pc = pc + immediate; // Not PC + 1 because it is already incremented in fetch()
            }
            break;
        case 5: // AND Operation
            // Perform bitwise AND operation
            result = value1 & value2;
            // Update flags
            // Negative Flag
            negativeFlag = (result < 0);
            if(negativeFlag)
                SREG = SREG | 0b00100000 ;
            // Zero Flag
            zeroFlag = (result == 0);
            if(zeroFlag)
                SREG = SREG | 0b00001000 ;
            break;
        case 6: // OR Operation
            // Perform bitwise OR operation
            result = value1 | value2;
            // Update flags
            // Negative Flag
            negativeFlag = (result < 0);
            if(negativeFlag)
                SREG = SREG | 0b00100000 ;
            // Zero Flag
            zeroFlag = (result == 0);
            if(zeroFlag)
                SREG = SREG | 0b00001000 ;
            break;
        case 7: // JUMP register Operation
            // Jump to the address stored in the given register
            pc = value1 || value2;
            break;
        case 8: // Shift Left Circular Operation
            // Perform circular left shift operation
            result = (value1 << immediate) | (value1 >> (8 - immediate));
            result &= ~(0xFF << (8 - immediate)); // Clear any sign bits
            // Update flags
            // Negative Flag
            negativeFlag = (result < 0);
            if(negativeFlag)
                SREG = SREG | 0b00100000 ;
            // Zero Flag
            zeroFlag = (result == 0);
            if(zeroFlag)
                SREG = SREG | 0b00001000 ;
            break;
        case 9: // Shift Right Circular Operation
            // Perform circular right shift operation
            result = (value1 >> immediate) & ~(0xFF << (8 - immediate)) | (value1 << (8 - immediate));
            // Update flags
            // Negative Flag
            negativeFlag = (result < 0);
            if(negativeFlag)
                SREG = SREG | 0b00100000 ;
            // Zero Flag
            zeroFlag = (result == 0);
            if(zeroFlag)
                SREG = SREG | 0b00001000 ;
            break;
        case 10: // LOAD Byte Operation
            // Load byte from Data Memory
            GPRS[R1] = DataMemory[immediate];
            // No flags are affected
            break;
        case 11: // Store Byte Operation
            // Store byte to Data Memory
            DataMemory[immediate] = value1;
            // No flags are affected
            break;
        default:
            printf("Invalid OPCode\n");
            break;    

    }

    if (opcode >= 0 && opcode <= 2 || opcode == 5 || opcode == 6 || opcode == 8 || opcode == 9) {

        // Write the result back to the destination register
        GPRS[R1] = result;
    }

}

void main(){
    
}