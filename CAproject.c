#include <stdio.h>
#include <stdint.h>

int pc = 0;
short int instructionMemory[1024];
int8_t DataMemory[2048];
int8_t GPRS[64];
int8_t SREG;
void decode(short int);
void execute(int,int8_t,int8_t,int);
void Fetch(){
    short int instruction=0;
    for(pc;pc<1024;pc++){
        instruction=instructionMemory[pc];
        decode(instruction);
    }
}
void decode(short int instruction){
    int opcode=((instruction& 0x000000FF) & ob00000000000000001111000000000000)>>12;
    int R1=((instruction& 0x000000FF) & ob000000000000000000001111111110000000)>>6;
    int R2=((instruction& 0x000000FF) & ob000000000000000000000000000111111111);
    int immediate=((instruction& 0x000000FF) & ob000000000000000000000000000111111111);
    int8_t value1=GPRS[R1];
    int8_t value2=GPRS[R2];
    execute(opcode,value1,value2,immediate);
}
void execute(int opcode,int8_t value1,int8_t value2,int immediate){

}

void main(){
    
}