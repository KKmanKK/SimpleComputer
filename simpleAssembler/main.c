#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define MEMORY_SIZE 100
int memory[MEMORY_SIZE];

int command_number(char command[]) {
    if (!strcmp(command, "READ")) {
        return 10;
    } else {
        if (!strcmp(command, "WRITE")) {
        return 11;
    } else {
        if (!strcmp(command, "LOAD")) {
        return 20;
    } else {
        if (!strcmp(command, "STORE")) {
        return 21;
    } else {
        if (!strcmp(command, "ADD")) {
        return 30;
    } else {
        if (!strcmp(command, "SUB")) {
        return 31;
    } else {
        if (!strcmp(command, "DIVIDE")) {
        return 32;
    } else {
        if (!strcmp(command, "MUL")) {
        return 33;
    } else {
        if (!strcmp(command, "JUMP")) {
        return 40;
    } else {
        if (!strcmp(command, "JNEG")) {
        return 41;
    } else {
        if (!strcmp(command, "JZ")) {
        return 42;
    } else {
        if (!strcmp(command, "HALT")) {
        return 43;
    } else {
        if (!strcmp(command, "NOT")) {
        return 51;
    } else {
        if (!strcmp(command, "AND")) {
        return 52;
    } else {
        if (!strcmp(command, "OR")) {
        return 53;
    } else {
        if (!strcmp(command, "XOR")) {
        return 54;
    } else {
        if (!strcmp(command, "JNS")) {
        return 55;
    } else {
        if (!strcmp(command, "JC")) {
        return 56;
    } else {
        if (!strcmp(command, "JNC")) {
        return 57;    
    } else {
        if (!strcmp(command, "JP")) {
        return 58;
    } else {
        if (!strcmp(command, "JNP")) {
        return 59;
    } else {
        if (!strcmp(command, "CHL")) {
        return 60;
    } else {
        if (!strcmp(command, "SHR")) {
        return 61;
    } else {
        if (!strcmp(command, "RCL")) {
        return 62;
    } else {
        if (!strcmp(command, "RCR")) {
        return 63;
    } else {
        if (!strcmp(command, "NEG")) {
        return 64;
    } else {
        if (!strcmp(command, "ADDC")) {
        return 65;
    } else {
        if (!strcmp(command, "SUBC")) {
        return 66;
    } else {
        if (!strcmp(command, "LOGLC")) {
        return 67;
    } else {
        if (!strcmp(command, "LOGRC")) {
        return 68;
    } else {
        if (!strcmp(command, "RCCL")) {
        return 69;
    } else {
        if (!strcmp(command, "RCCR")) {
        return 70;
    } else {
        if (!strcmp(command, "MOVA")) {
        return 71;
    } else {
        if (!strcmp(command, "MOVR")) {
        return 72;
    } else {
        if (!strcmp(command, "MOVCA")) {
        return 73;
    } else {
        if (!strcmp(command, "MOVCR")) {
        return 74;
    } else {
        if (!strcmp(command, "ADDC")) {
        return 75;
    } else {
        if (!strcmp(command, "SUBC")) {
        return 76; 
    } else {
        if (!strcmp(command, "=")) {
        return 100; 
    } else {
        return -1;
    }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
}

int translate (char line[]) {

    int index = 0, operand = 0;
    char command[6];


    if (strchr(line, '=')) {
        
        sscanf(line, "%d %5s %x", &index, command, &operand);       

        if (command_number(command) == 100 && operand <= 65535) {
            
            memory[index] = operand;
            return 0;
        }
        else return -1;
    }
    


    sscanf(line, "%d %5s %d", &index, command, &operand);

    int command_num = command_number(command);
    if (command_num == -1) return -1;



    memory[index] = (1 << 16) | (command_num << 8) | operand;
    return 0;
}

int main(int argc, char *argv[]) {

    if (!strcmp(argv[1], ".sa")) {
        printf("File error.");
        return -1;
    }
    
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }

    FILE *input, *output;

    input = fopen(argv[1], "r");

    if (input == NULL) {
        printf("File open error.");
        return -1;
    }

    char line[16]; 
        
    while (fgets(line, sizeof(line), input) != NULL) {
        translate(line);
    }

    output = fopen(argv[2], "wb");
    if (output != NULL) {
        fwrite(memory, 1, sizeof(memory), output);
        fclose(output);
        return 0;
    }
    else return -1;

    fclose(input);
}


    
    