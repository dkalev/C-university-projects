/**************************************************************************************
 * Disassembler and emulator for 8-bit computer with the following 
 * instruction set:
 *  000 HLT Halt execution of the program.
 *  001 LOD Load a copy of the value in the referenced memory location in the
 *  accumulator.
 *  010 LDC Load the constant value of the operand in the accumulator.
 *  011 STO Store a copy of the contents of the accumulator in the referenced memory
 *  location.
 *  100 ADD Add the value in the referenced memory location to the value in the
 *  accumulator; store result in the accumulator.
 *  101 SUB Subtract the value in the referenced memory location from the value in the
 *  accumulator; store result in the accumulator.
 *  110 JMP Jump to the referenced memory location if the value of the accumulator is a
 *  positive number (equal or greater than 0).
 *  111 JMZ Jump to the referenced memory location if the value of the accumulator is 0. 
 ***************************************************************************************/

#include <stdio.h>  /*printf*/
#include <string.h> /*strlen*/
#include <stdlib.h> /*malloc*/
#include <math.h>   /*pow*/
#include <ctype.h>  /*isalnum*/

#define NUM_SELLS 32
#define WORD_LEN 8
#define FILENAME "data"

void initialize_memory_and_registers();
void decimal_to_binary(int dec, char bin[], int bits);
int signed_binary_to_decimal(char bin[]);
int unsigned_binary_to_decimal(char bin[]);
void load_default_memory();
void load_from_file();
void read_from_console();
void convert_to_assembly();
void display_memory();
void run(); 

char memory[NUM_SELLS][WORD_LEN+1];
char AC[WORD_LEN+1];
char IR[WORD_LEN+1];
int PC;


/*Initializes the memory, loads instructions into memory based on the command line arguments provided(-d for default memory, -c for 
console input and -f for loading memory form file). Then executes the program and displays the content of the memory in mashine code
and in assembly and the value in the AC register*/
int main(int argc, char *argv[]){
    initialize_memory_and_registers();
    
    if(argc == 1){
        printf("Usage:\n-f to read from file\n-d to use default memory\n-c to read from console\n");
        exit(1);
    }else if(strcmp(argv[1], "-d") == 0){
        load_default_memory();
    }else if(strcmp(argv[1], "-c") == 0){
        read_from_console();
    }else if(strcmp(argv[1], "-f") == 0){
        load_from_file();
    }
 
    run();
    printf("Memory contents\n\n");
    display_memory();
    printf("Contents of memory converted to assembly language\n\n");
    convert_to_assembly();
    int ACint = signed_binary_to_decimal(AC);
    printf("\nAC: %d\n", ACint);
    
    return 0;
}

/* Loads the instructions from a file and
 stores its contents into the memory */
void load_from_file(){
    FILE *file = fopen(FILENAME, "r");
    char line[WORD_LEN+1];
    
    if(file == NULL){
        printf("Failed to open file.\n");
        exit(1);
    }
    
    int i = 0, j;
    while(fgets(line, sizeof(line), file) && i < NUM_SELLS){
        
        if(isspace(line[0])) continue;
        
        int k = 0;
        for(j = 0; j < WORD_LEN; j ++){
            memory[i][k] = line[j];
                k++;
        }
        memory[i][k] = '\0';
        i++;
    }
    
    fclose(file);
}

/* Runs the program stored in memory */
void run(){
    PC = 0;
    int stop = 0;
    int jumped;
    
    while(!stop){
        jumped = 0;
        
        strncpy(IR, memory[PC], WORD_LEN);
        char *operand = IR + 3;
        char *opcode = (char *)malloc(4);
        opcode[3] = '\0';
        strncpy(opcode, IR, 3);
        int opcodeInt = unsigned_binary_to_decimal(opcode);
        int operandInt;
        
        
        switch(opcodeInt){
            case 0:/*HLT*/
                stop = 1;
                break;
            case 1:/*LOD*/
                operandInt = unsigned_binary_to_decimal(operand);
                strncpy(AC, memory[operandInt], WORD_LEN);
                break;
            case 2:/*LDC*/
                operandInt = signed_binary_to_decimal(operand);
                decimal_to_binary(operandInt, AC, WORD_LEN);
                break;
            case 3:/*STO*/
                operandInt = unsigned_binary_to_decimal(operand);
                strncpy(memory[operandInt], AC, WORD_LEN);
                break;
            case 4:/*ADD*/
                operandInt = unsigned_binary_to_decimal(operand);
                int add1 = signed_binary_to_decimal(memory[operandInt]);
                int add2 = signed_binary_to_decimal(AC);
                add1 += add2;
                decimal_to_binary(add1, AC, WORD_LEN);
                break;
            case 5:/*SUB*/
                operandInt = unsigned_binary_to_decimal(operand);
                int sub1 = signed_binary_to_decimal(memory[operandInt]);
                int sub2 = signed_binary_to_decimal(AC);
                sub2 -= sub1;
                decimal_to_binary(sub2, AC, WORD_LEN);
                break;
            case 6:/*JMP*/
                if(signed_binary_to_decimal(AC) >= 0){
                    operandInt = unsigned_binary_to_decimal(operand);
                    PC = operandInt;
                    jumped = 1;    
                } 
                break;
            case 7:/*JMZ*/
                if(signed_binary_to_decimal(AC) == 0){
                    operandInt = unsigned_binary_to_decimal(operand);
                    PC = operandInt;
                    jumped = 1;
                }
                break;
            default:
                printf("garbage value\n");
        }
        
        if(!jumped) PC = (PC + 1) % NUM_SELLS;
        free(opcode);
    }
}


/* Converts the memory contents to assembly language and
displays the results on the screen */
void convert_to_assembly(){
    
    int i;
    int n = NUM_SELLS;
    for(i = 0; i < n; i++){
        const char *instr = memory[i];
        char *copy = (char *)malloc(WORD_LEN+1);
        strncpy(copy, instr, WORD_LEN+1);
        copy[WORD_LEN]='\0';
        char *operand = copy + 3;
        char *opcode = (char *)malloc(4);
        strncpy(opcode, copy, 4);
        opcode[3] = '\0';
        int opcodeInt = unsigned_binary_to_decimal(opcode);
        int operandInt = 0;
        
        free(opcode);
        
        if(i < 10){
            printf("0%d: ", i);
        }else{
            printf("%d: ", i);
        }
        
        switch(opcodeInt){
            case 0:
                if(strcmp(operand, "00000") == 0){
                    printf("HLT\n"); 
                }else{
                    printf("%d\n",unsigned_binary_to_decimal(copy));   
                }
                break;
            case 1:
                operandInt = unsigned_binary_to_decimal(operand);
                printf("LOD %d\n",operandInt );
                break;
            case 2:
                operandInt = signed_binary_to_decimal(operand);
                printf("LDC %d\n",operandInt );
                break;
            case 3:
                operandInt = unsigned_binary_to_decimal(operand);
                printf("STO %d\n",operandInt );
                break;
            case 4:
                operandInt = unsigned_binary_to_decimal(operand);
                printf("ADD %d\n",operandInt );
                break;
            case 5:
                operandInt = unsigned_binary_to_decimal(operand);
                printf("SUB %d\n",operandInt );
                break;
            case 6:
                operandInt = unsigned_binary_to_decimal(operand);
                printf("JMP %d\n",operandInt );
                break;
            case 7:
                operandInt = unsigned_binary_to_decimal(operand);
                printf("JMZ %d\n",operandInt );
                break;
            default:
                printf("garbage value\n");
        }
        free(copy);
    }
}

/* Prompts the user to input the memory contents from the console
(i.e. command line),
reads the input in and stores it into the memory */
void read_from_console(){
    
    int flag = 1;
    int mem_sell = 0;
    
    printf("\nEnter an instruction(%d bit binary number) for the correspondent memory location ot type 'quit' to exit\n", WORD_LEN);
    
    while(flag && mem_sell < NUM_SELLS){
        printf("Memory address %d:", mem_sell);
        char input[WORD_LEN+1];
        char c;
        int i = 0;
        while((c = getchar()) != '\n'){
            if(isalnum(c)){
                input[i] = c;
                i++;
            }
        }
        input[i] = '\0';
        
        if (strcmp(input, "quit") == 0){
            flag = 0;
        }else{
            int j;
            int valid = 1;
            for(j = 0; j < WORD_LEN; j++){
                if(input[j] == '0' || input[j] == '1'){
                  memory[mem_sell][j] = input[j];  
                }else{
                    valid = 0;
                } 
            }
            memory[mem_sell][j] = '\0';
            if(valid){
                mem_sell++;
            }
        }
    }
}


/* Displays the memory contents & respective locations */
void display_memory(){
    int i, j;
    for(i = 0; i < NUM_SELLS; i++){
        if(i < 10){
            printf("0%d: ", i);
        }else{
            printf("%d: ", i);
        }
        printf("%s\n", memory[i]);
    }
}

/* Loads some default content into the memory */
void load_default_memory(){
    char* instructions[] = {"01000101", "01111110", "01001101", 
                    "10011110", "01111111", "00000000"};

    int i, j, n;
    for(i = 0, n = sizeof(instructions)/WORD_LEN; i < n; i++){
        for(j = 0; j < WORD_LEN; j++){
            memory[i][j] = instructions[i][j];
        }
        memory[i][j] = '\0';
    }
}

/*
converts a decimal number to an signed int with specified size(number of bits) and stores every bit as a char in a char array
can overflow
*/
void decimal_to_binary(int dec, char bin[],int bits){
    int i = bits -1;
    while(i >= 0){
        bin[i] = (dec & 1) + '0';
        dec >>= 1;
        i--;
    }
}


/*converts a signed int(bits are stored as chars in a char array) to a decimal number and returns it as an int*/
int signed_binary_to_decimal(char bin[]){
    int n = strlen(bin) - 1;
    int dec = 0;
    int base = 1;
    if(bin[0] == '1') dec = pow(2, n);
    while(n > 0){
        dec = dec + (bin[n] - 48)*base;
        base *= 2;
        n--;
    }
    return dec;
}

/*converts an unsigned int(bits are stored as chars in a char array) to a decimal number and returns it as an int*/
int unsigned_binary_to_decimal(char bin[]){
    int n = strlen(bin) - 1;
    int dec = 0;
    int base = 1;
    while(n >= 0){
        dec = dec + (bin[n] - 48)*base;
        base *= 2;
        n--;
    }
    return dec;
}

/*Initializes the memory array to '0'*/
void initialize_memory_and_registers(){
    int i;
    int j;
    for(i = 0; i < NUM_SELLS; i++){
        for(j=0; j < WORD_LEN; j++){
            memory[i][j] = '0';
        }
        memory[i][j]='\0';
    }
    for(i = 0; i < WORD_LEN; i++){
        AC[i] = '0';
        IR[i] = '0';
    }
    AC[i] = '\0';
    IR[i] = '\0';
}