#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int pc = 0;
short int instructionMemory[1024];
int8_t DataMemory[2048];
int8_t GPRS[64];
int8_t SREG;
short int instruction = -4096;
int j = 0;
bool empty = false;
int value[6] = {-1, 0, 0, 0, 0, 0};
char *String;
bool branch;
void decode(short int);
void execute(int value[6]);
void Fetch()
{
    if (instructionMemory[pc] == -4096)
    {
        empty = true;
        instruction = -4096;
    }
    //  strncpy(String, "Instruction %d executed \n",pc);

    else
    {
        instruction = instructionMemory[pc];
        printf("Intruction %d fetched.\n", pc + 1);
    }
    //  pc++;
    pc++;
}

void decode(short int instruction)
{
    if (instruction == -4096)
    {
        value[0] = -1;
    }
    else
    {
        printf("Intruction %d decoded.\n", pc);
        // Opcode
        value[0] = (instruction & 0b1111000000000000) >> 12;
        // R1
        value[1] = (instruction & 0b0000111111000000) >> 6;
        // R2
        value[2] = (instruction & 0b0000000000111111);
        // Immediate
        value[3] = (instruction & 0b0000000000111111);
        // value inside register R1
        value[4] = GPRS[value[1]];
        // value inside register R2
        value[5] = GPRS[value[2]];
        // printf("Instruction %i\n", pc);
        printf("---------- \n");
        printf("opcode = %i\n", value[0]);
        printf("R1 = %i\n", value[1]);
        printf("R2 = %i\n", value[2]);
        printf("immediate = %i\n", value[3]);
        printf("value[R1] = %i\n", value[4]);
        printf("value[R2] = %i\n", value[5]);
        printf("---------- \n\n");
        // strncpy(String, "Instruction %d executed \n",pc);
    }
}

void execute(int value[6])
{
    if (value[0] != -1)
    {
        int8_t result;
        SREG = 0b00000000;

        int carryFlag = 0;
        int overflowFlag = 0;
        int negativeFlag = 0;
        int signFlag = 0;
        int zeroFlag = 0;
        printf("Intruction %d executed.\n", pc - 1);
        printf("---------- \n");
        switch (value[0])
        {
        case 0: // ADD Operation
            // Perform addition
            result = value[4] + value[5];
            printf("Register %d updated, new value : %d.\n", value[1], result);
            // Update flags
            // Carry Flag
            carryFlag = (((uint16_t)value[4] + (uint16_t)value[5]) > UINT8_MAX);
            if (carryFlag)
                SREG = SREG | 0b00010000;
            // Overflow Flag
            overflowFlag = ((value[4] > 0 && value[5] > 0 && result < 0) || (value[4] < 0 && value[5] < 0 && result > 0));
            if (overflowFlag)
                SREG = SREG | 0b00001000;
            // Negative Flag
            negativeFlag = (result < 0);
            if (negativeFlag)
                SREG = SREG | 0b00000100;
            // Sign Flag
            signFlag = ((negativeFlag ^ overflowFlag) != 0);
            if (signFlag)
                SREG = SREG | 0b00000010;
            // Zero Flag
            zeroFlag = (result == 0);
            if (zeroFlag)
                SREG = SREG | 0b00000001;
            break;
        case 1: // Subtract Operation
            // Perform subtraction
            result = value[4] - value[5];
            printf("Register %d updated, new value : %d.\n", value[1], result);
            // Update flags
            // Overflow Flag
            overflowFlag = ((value[4] < 0 && value[5] > 0 && result > 0) || (value[4] > 0 && value[5] < 0 && result < 0));
            if (overflowFlag)
                SREG = SREG | 0b00001000;
            // Negative Flag
            negativeFlag = (result < 0);
            if (negativeFlag)
                SREG = SREG | 0b00000100;
            // Sign Flag
            signFlag = ((negativeFlag ^ overflowFlag) != 0);
            if (signFlag)
                SREG = SREG | 0b00000010;
            // Zero Flag
            zeroFlag = (result == 0);
            if (zeroFlag)
                SREG = SREG | 0b00000001;
            break;
        case 2: // Multiply Operation
            // Perform multiplication
            result = value[4] * value[5];
            printf("Register %d updated, new value : %d.\n", value[1], result);
            // Update flags
            // Negative Flag
            negativeFlag = (result < 0);
            if (negativeFlag)
                SREG = SREG | 0b00000100;
            // Zero Flag
            zeroFlag = (result == 0);
            if (zeroFlag)
                SREG = SREG | 0b00000001;
            break;
        case 3: // Load Immediate Operation
            // Load immediate value into register
            GPRS[value[1]] = value[3];
            printf("Register %d Loaded with value : %d.\n", value[1], value[3]);
            // No flags are affected
            break;
        case 4: // Branch If Equal Zero Operation
            if (value[4] == 0)
            {
                // Update PC to immediate value
                pc = pc + value[3]; // Not PC + 1 because it is already incremented in fetch()
                branch = true;
            }
            break;
        case 5: // AND Operation
            // Perform bitwise AND operation
            result = value[4] & value[5];
            printf("Register %d updated, new value : %d.\n", value[1], result);
            // Update flags
            // Negative Flag
            negativeFlag = (result < 0);
            if (negativeFlag)
                SREG = SREG | 0b00000100;
            // Zero Flag
            zeroFlag = (result == 0);
            if (zeroFlag)
                SREG = SREG | 0b00000001;
            break;
        case 6: // OR Operation
            // Perform bitwise OR operation
            result = value[4] | value[5];
            printf("Register %d updated, new value : %d.\n", value[1], result);
            // Update flags
            // Negative Flag
            negativeFlag = (result < 0);
            if (negativeFlag)
                SREG = SREG | 0b00000100;
            // Zero Flag
            zeroFlag = (result == 0);
            if (zeroFlag)
                SREG = SREG | 0b00000001;
            break;
        case 7: // JUMP register Operation
            // Jump to the address stored in the given register
            pc = value[4] || value[5];
            branch = true;
            break;
        case 8: // Shift Left Circular Operation
            // Perform circular left shift operation
            result = (value[4] << value[3]) | (value[4] >> (8 - value[3]));
            printf("Register %d updated, new value : %d.\n", value[1], result);
            result &= ~(0xFF << (8 - value[3])); // Clear any sign bits
            // Update flags
            // Negative Flag
            negativeFlag = (result < 0);
            if (negativeFlag)
                SREG = SREG | 0b00000100;
            // Zero Flag
            zeroFlag = (result == 0);
            if (zeroFlag)
                SREG = SREG | 0b00000001;
            break;
        case 9: // Shift Right Circular Operation
            // Perform circular right shift operation
            result = (value[4] >> value[3]) & ~(0xFF << (8 - value[3])) | (value[4] << (8 - value[3]));
            printf("Register %d updated, new value : %d.\n", value[1], result);
            // Update flags
            // Negative Flag
            negativeFlag = (result < 0);
            if (negativeFlag)
                SREG = SREG | 0b00000100;
            // Zero Flag
            zeroFlag = (result == 0);
            if (zeroFlag)
                SREG = SREG | 0b00000001;
            break;
        case 10: // LOAD Byte Operation
            // Load byte from Data Memory
            GPRS[value[1]] = DataMemory[value[3]];
            printf("Register %d Loaded from Data Memory, new value  : %d.\n", value[1], DataMemory[value[3]]);
            // No flags are affected
            break;
        case 11: // Store Byte Operation
            // Store byte to Data Memory
            DataMemory[value[3]] = value[4];
            printf("Data Stored at index  %d in Data Memory, value  : %d.\n", value[3], value[4]);
            // No flags are affected
            break;
        default:
            printf("Invalid OPCode\n");
            break;
        }
        if (value[0] >= 0 && value[0] <= 2 || value[0] == 5 || value[0] == 6 || value[0] == 8 || value[0] == 9)
        {

            // Write the result back to the destination register
            GPRS[value[1]] = result;
        }

        printf("SREG = %i\n", SREG);
        printf("---------- \n\n");
        // strncpy(String, "Instruction %d executed \n",pc);
    }
}

// from decimal to 6 bits binary string
char binaryString[7];

char *decimalToBinary(int decimal)
{
    // Iterate through each bit position from left to right
    for (int i = 5; i >= 0; i--)
    {
        // Extract the current bit (0 or 1) using bitwise AND
        int bit = (decimal >> i) & 1;

        // Append the bit to the binary string
        binaryString[5 - i] = bit + '0'; // Convert the bit to ASCII character ('0' or '1')
    }

    // Add null terminator to the end of the string
    binaryString[6] = '\0';

    // Return the global binary string
    return binaryString;
}

// From String to 16 bits for instruction memory
short int binaryToShort(char *binaryString)
{
    short int result = 0;
    int length = strlen(binaryString);
    for (int i = 0; i < length; i++)
    {
        result = (result << 1) + (binaryString[i] - '0');
    }
    return result;
}

int LoadInstruction()
{
    for (int i = 0; i < 1024; i++)
    {
        instructionMemory[i] = -4096;
    }
    FILE *file;
    char line[256];
    char *split;
    char *ptr;
    // Open CMakeLists.txt file
    char filename[] = "CMakeLists.txt"; // Relative path to the file

    // Open the file
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    while (fgets(line, sizeof(line), file))
    {

        if (strspn(line, " \t\n") == strlen(line))
        {
            // Line is empty, skip processing
            continue;
        }
        // printf("line: %s ",line);
        int i = 3;
        while (i > 0)
        {
            split = strtok(line, " ");
            // Print split to see if it contains expected values

            char instruction[16] = "";

            while (split != NULL)
            {
                {
                    // printf("split: %s\n", split);

                    if (i == 3)
                    {
                        if (strcmp(split, "ADD") == 0)
                        {
                            strcpy(instruction, "0000");
                        }
                        else if (strcmp(split, "SUB") == 0)
                        {
                            strcpy(instruction, "0001");
                        }
                        else if (strcmp(split, "MUL") == 0)
                        {
                            strcpy(instruction, "0010");
                        }
                        else if (strcmp(split, "LDI") == 0)
                        {
                            strcpy(instruction, "0011");
                        }
                        else if (strcmp(split, "BEQZ") == 0)
                        {
                            strcpy(instruction, "0100");
                        }
                        else if (strcmp(split, "AND") == 0)
                        {
                            strcpy(instruction, "0101");
                        }
                        else if (strcmp(split, "OR") == 0)
                        {
                            strcpy(instruction, "0110");
                        }
                        else if (strcmp(split, "JR") == 0)
                        {
                            strcpy(instruction, "0111");
                        }
                        else if (strcmp(split, "SLC") == 0)
                        {
                            strcpy(instruction, "1000");
                        }
                        else if (strcmp(split, "SRC") == 0)
                        {
                            strcpy(instruction, "1001");
                        }
                        else if (strcmp(split, "LB") == 0)
                        {
                            strcpy(instruction, "1010");
                        }
                        else if (strcmp(split, "SB") == 0)
                        {
                            strcpy(instruction, "1011");
                        }
                        else
                        {
                            printf("Invalid operator entered\n");
                            return 1;
                        }
                    }
                    if (i == 2)
                    {
                        ptr = strchr(split, 'R');
                        if (ptr != NULL)
                        {
                            // If 'R' is found, extract the number
                            int number = atoi(ptr + 1);
                            if (number > 63)
                            {
                                printf("register number invalid\n");
                                return 1;
                            }
                            char *binaryString = decimalToBinary(number);
                            strcat(instruction, binaryString);
                        }
                        else
                        {
                            printf("Invalid assembly,R is not found\n");
                            return 1;
                        }
                    }

                    // Add flag to check if immediate is valid according to above switch case of alu
                    if (i == 1)
                    {
                        ptr = strchr(split, 'R');
                        if (ptr != NULL)
                        {
                            // If 'R' is found, extract the number
                            int number = atoi(ptr + 1);
                            if (number > 63)
                            {
                                printf("register number invalid\n");
                                return 1;
                            }
                            char *binaryString = decimalToBinary(number);
                            strcat(instruction, binaryString);
                        }
                        else
                        {
                            int number = atoi(split);
                            if (number > 63)
                            {
                                printf("immediate number more than 6 bits\n");
                                return 1;
                            }
                            char *binaryString = decimalToBinary(number);
                            strcat(instruction, binaryString);
                        }
                    }
                    i--;
                    //         // Get the next token
                    split = strtok(NULL, " ");
                }
            }

            printf("Instruction %d : %s\n", j, instruction);
            short int temp = binaryToShort(instruction);
            // printf("Instruction %d : %d\n",j,temp);
            instructionMemory[j] = temp;
            j++;
        }
    }
    fclose(file);
}
void pipeline()
{
    bool empty2 = false;
    bool empty3 = false;
    int i = 1;
    for (pc; pc < 1024; i++)
    {
        if (empty3)
        {
            break;
        }
        printf("Cycle %d :\n", i);
        if (!branch)
        {
            execute(value);
        }
        else
        {
            branch = false;
        }

        // if(branch){
        //     branch=false;
        //     Fetch();
        // }
        if (!branch)
        {
            decode(instruction);
        }
        Fetch();
        if (empty)
        {
            if (empty2)
            {
                empty3 = true;
            }
            empty2 = true;
        }
        printf("\n");
    }
    printf("Executed all loaded Instructions.\n");
    // Register Prints
     for (int i = 0; i < j + 1; i++)
     {
         printf("Register Value %d is : %d\n", i, GPRS[i]);
     }
     printf("PC value : %d\n",pc);
     printf("Last SREG valie : %d\n",SREG);

    // Instruction Prints
     for (int i = 0; i < j+1; i++)
     {
         printf("Instruction %d is : %d\n", i, instructionMemory[i]);
     }

    // Data Memory Prints
     for (int i = 0; i < 2048; i++)
     {
         printf("Data Memory %d is : %d\n", i, DataMemory[i]);
     }
     printf("%s\n", String);
}

int main()
{
    if (LoadInstruction() == 1)
    {
        return 0;
    }
    // for (int i = 0; i < j+1; i++)
    // {
    //     printf("Instruction %d is : %d\n", i, instructionMemory[i]);
    // }
    pipeline();
}
