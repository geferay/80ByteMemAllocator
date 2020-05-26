/*This is the main code file for the 80ByteMemAllocator
 This program was made as part of an assignment for CSS 430 Operating Systems
 at The University of Washington, Bothell, for Spring Quarter 2020.
 
 Author: Geoffrey Powell-Isom
 Date:   2020-05-25, Monday
 File:   main.cpp
 Build:  gcc -o 80BMAloc main.c
 Run:    ./80BMAloc
 */

/*Include files*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*Global constants*/
#define DEBUG_LEVEL 0
#define INPUT_MAX 40
/*We got to have a version that's +1, for the null terminator.*/
#define INPUT_LEN 41
#define MEMORY_SIZE 80
#define MEMORY_LEN 81

/*custom struct*/
typedef struct {
    size_t inputMax;
    size_t charInput;        /*getline() tells us the input size*/
    char   input[INPUT_LEN]; /*Way too much space. The likely max needed is 9.*/
    char   command;
    char   program;
    int    amount;
    char   algorithm;
    char*  parsed;
    int    curr;             /*current position in input string*/
    int    errorFound;
} dataStruct;


/*Function Prototypes*/
void compactMemory(char *memoryPool);
void freeMemory(char *memoryPool, char program);
void runStuff(dataStruct * theData, char * memoryPool);
void firstFit(char *memoryPool, char program, int amount);
void bestFit(char *memoryPool, char program, int amount);
void worstFit(char *memoryPool, char program, int amount);
void executionFunction(dataStruct * theData, char * memoryPool, char * scriptFile);
void stdinRunner(dataStruct * theData, char * memoryPool);
void fileRunner(dataStruct * theData, char * memoryPool, char * scriptFile);

/*Main function*/
/*We don't worry about input. It will just be ignored.*/
int main(){
    printf("Welcome to the 80ByteMemAllocator Demo program, an assignment for CSS 430 (Operating Systems) at UWB.\n");
    printf("You are here, I guess.\n");
    
    const char *memoryTemplate = "................................................................................";
    char *memoryPool;
    memoryPool = (char*)malloc(MEMORY_LEN * sizeof(char));
    memoryPool[MEMORY_LEN - 1] = '\0'; /*set the 81st character to the null character, because it's the end of a string*/
    for(int i = 0; memoryTemplate[i]; ++i) memoryPool[i] = memoryTemplate[i];
    
    /*main program loop code*/
    int notDone = 1; /*We're never done, unless somebody implements an exit command or hits Ctr-C*/
    dataStruct * theData = (dataStruct*)malloc(sizeof(dataStruct));
    while(notDone){
        theData->inputMax   = INPUT_MAX;
        theData->charInput  =  0;        /*getline() tells us the input size*/
        theData->command    = '\0';
        theData->program    = '\0';
        theData->amount     =    0;
        theData->algorithm  = '\0';
        theData->parsed     = NULL;
        theData->curr       = 0;         /*current position in input string*/
        theData->errorFound = 0;
        
        stdinRunner(theData, memoryPool);
    }
    
    if(memoryPool){
        free(memoryPool);
        memoryPool = NULL;
    }
    
    if(theData){
        free(theData);
        theData = NULL;
    }
    return 0; /*Every day's a good day! :-) */
}


/*function to run commands from stdin*/
void stdinRunner(dataStruct * theData, char * memoryPool){
    printf("Give a command: ");
    char *inp = theData->input; /*ugly hack to get getline() to be happy*/
    theData->charInput = getline(&inp, &theData->inputMax, stdin);
    if(theData->charInput <= 1) return; /*nothing to do here, just repeat the loop*/
    executionFunction(theData, memoryPool, theData->input);
}


/*function to run commands from file*/
void fileRunner(dataStruct * theData, char * memoryPool, char * scriptFile){
    FILE *in;
    if(DEBUG_LEVEL >= 1) printf("Loading file: %s\n", scriptFile);
    in = fopen(scriptFile,"r");
    if(!in){
        printf("We're sorry, but the file failed to load. :-(\nDon't give up!\n");
        return; /*get out of here*/
    }
    
    while(fgets(theData->input,INPUT_LEN,in) != NULL){
        /*reset our variables*/
        theData->charInput  =  0;        /*we will store the input length*/
        theData->command    = '\0';
        theData->program    = '\0';
        theData->amount     =    0;
        theData->algorithm  = '\0';
        theData->parsed     = NULL;
        theData->curr       = 0;         /*current position in input string*/
        theData->errorFound = 0;
        
        for(int i = 0; theData->input[i]; ++i) ++theData->charInput;
        if(theData->charInput <= 1) continue; /*nothing to do here, just repeat the loop*/
        if(DEBUG_LEVEL >= 2) printf("read line: %s\n", theData->input);
        
        
        if(DEBUG_LEVEL >= 2) printf("ENTERING executionFunction(). theData->input = %s\n", theData->input);
        executionFunction(theData, memoryPool, theData->input);
    }
    
    fclose(in);
}


/*function to execute commands*/
void executionFunction(dataStruct * theData, char * memoryPool, char * command){
    /*remove trailing newline* - It may be: LF, CR, CR+LF, or any combination of 3 LF/CR characters.*/
    if((theData->input[theData->charInput - 1] == 10) || (theData->input[theData->charInput - 1] == 13)) theData->input[--theData->charInput] = '\0';
    if((theData->input[theData->charInput - 1] == 10) || (theData->input[theData->charInput - 1] == 13)) theData->input[--theData->charInput] = '\0';
    if((theData->input[theData->charInput - 1] == 10) || (theData->input[theData->charInput - 1] == 13)) theData->input[--theData->charInput] = '\0';
    
    if(DEBUG_LEVEL >= 2) printf("End-lines removed. theData->input = %s\n", theData->input);
    
    /*skip leading spaces*/
    while(theData->input[theData->curr] == ' ' || theData->input[theData->curr] == 9) ++theData->curr;
    if(DEBUG_LEVEL >= 1) printf("File processing: theData->input = %s\n", theData->input);
    /*assign command*/
    theData->command = theData->input[theData->curr++];
    
    /*parsing section===============================================================================*/
    if(theData->command == 'R' || theData->command == 'r'){
        theData->command = 'R';
        /*error on extra non-space characters*/
        if(theData->input[theData->curr] != ' ' && theData->input[theData->curr] != 9) theData->errorFound = 1;
        if(!theData->errorFound){
            /*skip space characters and tabs*/
            while(theData->input[theData->curr] == ' ' || theData->input[theData->curr] == 9) ++theData->curr;
            /*Get the file name*/
            char *fileName;
            int fileCurr = 0;
            fileName = (char*)malloc((INPUT_LEN - 2)*sizeof(char));
            while(theData->input[theData->curr]) fileName[fileCurr++] = theData->input[theData->curr++];
            fileName[fileCurr] = '\0';
            if(DEBUG_LEVEL >= 2) printf("The file name received is \"%s\"\n", fileName);
            /*read the commands from a file*/
            theData->parsed = fileName;
        }
    }
    else if(theData->command == 'S' || theData->command == 's'){
        theData->command = 'S';
    }
    else if(theData->command == 'C' || theData->command == 'c'){
        theData->command = 'C';
    }
    else if(theData->command == 'F' || theData->command == 'f'){
        theData->command = 'F';
        /*error on extra non-space characters*/
        if(theData->input[theData->curr] != ' ' && theData->input[theData->curr] != 9) theData->errorFound = 1;
        if(!theData->errorFound){
            /*skip spaces and tabs*/
            while(theData->input[theData->curr] == ' ' || theData->input[theData->curr] == 9) ++theData->curr;
            /*assign program*/
            theData->program = theData->input[theData->curr];
            /*make uppercase*/
            if(theData->program > 90) theData->program -= 32;
        }
    }
    else if(theData->command == 'A' || theData->command == 'a'){
        theData->command = 'A';
        if(DEBUG_LEVEL >= 2) printf("Running memory allocator %s\n", (theData->input + theData->curr));
        /*error on extra non-space characters*/
        if(theData->input[theData->curr] != ' ' && theData->input[theData->curr] != 9) theData->errorFound = 1;
        if(!theData->errorFound){
            /*skip spaces and tabs*/
            while(theData->input[theData->curr] == ' ' || theData->input[theData->curr] == 9) ++theData->curr;
            
            /*assign program*/
            theData->program = theData->input[theData->curr++];
            
            /*make uppercase*/
            if(theData->program > 90) theData->program -= 32;
            
            /*error on extra non-space characters*/
            if(theData->input[theData->curr] != ' ' && theData->input[theData->curr] != 9) theData->errorFound = 1;
            if(!theData->errorFound){
                /*skip spaces and tabs*/
                while(theData->input[theData->curr] == ' ' || theData->input[theData->curr] == 9) ++theData->curr;
                
                if(DEBUG_LEVEL >= 1) printf("Running memory allocator %s\nProgram is %c\n", (theData->input + theData->curr), theData->program);
                /*get integer from text, for space size to allocate*/
                int num = 0;
                int mult = 0;
                while((theData->input[theData->curr] >= '0') && (theData->input[theData->curr] <= '9')){
                    if(DEBUG_LEVEL >= 2) printf("num is %d, mult is %d, input[%d] is %c\n", num, mult, theData->curr, theData->input[theData->curr]);
                    if(mult) num *= 10;
                    num += theData->input[theData->curr++] - 48;
                    mult = 1;
                }
                if(DEBUG_LEVEL >= 2) printf("Number is %d\n", num);
                theData->amount = num;
                /*error on extra non-space characters*/
                if(theData->input[theData->curr] != ' ' && theData->input[theData->curr] != 9) theData->errorFound = 1;
                if(!theData->errorFound){
                    
                    /*skip spaces and tabs*/
                    while(theData->input[theData->curr] == ' ' || theData->input[theData->curr] == 9) ++theData->curr;
                    /*assign algorithm*/
                    theData->algorithm = theData->input[theData->curr++];
                    /*make uppercase*/
                    if(theData->algorithm > 90) theData->algorithm -= 32;
                }
            }
        }
    }
    else{
        theData->errorFound = 1; /*There must have been an error.*/
    }
    
    runStuff(theData, memoryPool);
    
    /*return nothing;*/
    
}


/*function to runStuff*/
void runStuff(dataStruct * theData, char * memoryPool){
    
    if(DEBUG_LEVEL >= 1) printf("Command is: %c\n", theData->command);
    
    if(theData->errorFound == 1){
        /*show error*/
        printf("We're dearly sorry, but your command was not recognized.\n");
        printf("The unrecognized command was: %s\n", theData->input);
        printf("Please check your sytax and try again.\n");
        theData->errorFound = 0; /*leaving this set will confuse the program*/
    }
    else{
        /*run commands*/
        switch(theData->command){
            case 'R':{
                /*setup reading from the file*/
                /*run commands from file*/
                fileRunner(theData, memoryPool, theData->parsed);
            } break;
            case 'S':{
                /*show the state of the memory pool*/
                printf("%s\n", memoryPool);
            } break;
            case 'C':{
                /*compact the memory*/
                compactMemory((char*)memoryPool);
            } break;
            case 'F':{
                /*compact the memory*/
                freeMemory((char*)memoryPool, theData->program);
            } break;
            case 'A':{
                if(DEBUG_LEVEL >= 1) printf("Running memory allocator %c to allocate %d bytes to program %c.\n", theData->algorithm, theData->amount, theData->program);
                switch(theData->algorithm){
                    case 'F':{
                        firstFit(memoryPool, theData->program, theData->amount);
                    } break;
                    case 'B':{
                        bestFit(memoryPool, theData->program, theData->amount);
                    } break;
                    case 'W':{
                        worstFit(memoryPool, theData->program, theData->amount);
                    } break;
                    default:{
                        printf("The memory allocation algorithm %c is not implemented. Please try another algorithm.\n", theData->algorithm);
                    } break;
                }
            } break;
            default:{
                /*This is a previously uncaught error.*/
                printf("Something strange has happened, and the request cannot be fulfilled. Perhaps there was something wrong with the syntax, or maybe something more unknown. Please check to make sure everything is in order, and try again.\n");
            } break;
        }
    }
    /*return nothing;*/
}


/*function to allocate memory with firstFit algorithm*/
void firstFit(char *memoryPool, char program, int amount){
    int location = 0;
    for(int i = 0; i < MEMORY_SIZE && (amount - (i - location)); ++i){
        if(DEBUG_LEVEL >= 2) printf("We're in the firstFit() for() loop.\n"); fflush(stdout);
        if(memoryPool[i] != '.') location = i + 1; /*location not empty*/
    }
    if(MEMORY_SIZE >= (location + amount)){
        for(int i = 0; i < amount; ++i, ++location){
            memoryPool[location] = program;
        }
    }
    else{
        printf("I'm sorry, Dave. I can't allocate enough memory for that. %d bytes for program %c is more than I can remember.\n", amount, program);
    }
    /*return nothing;*/
}


/*function to allocate memory with bestFit algorithm*/
void bestFit(char *memoryPool, char program, int amount){
    int beg = 0;
    int end = 90;
    int testBeg = 0;
    int testEnd = 0;
    
    int location = 0;
    for(int i = 0; i < MEMORY_SIZE; ++i){
        if(DEBUG_LEVEL >= 2) printf("We're in the bestFit() for() loop.\n"); fflush(stdout);
        if(memoryPool[i] != '.'){
            if((testEnd - testBeg) >= amount && (testEnd - testBeg) < (end - beg)){
                beg = testBeg;
                end = testEnd;
            }
            testBeg = i + 1; /*looking for a new beginning*/
        }
        testEnd = i; /*testEnd follows i*/
    }
    /*If the memoryPool is free at the end, this test won't automatically run in the for() loop.
     So, we have to run it manually, here.*/
    if((testEnd - testBeg) >= amount && (testEnd - testBeg) < (end - beg)){
        beg = testBeg;
        end = testEnd;
    }
    if(end < 90){  /*We can allocate, if we've changed from the default value of end.*/
        location = beg;
        for(int i = 0; i < amount; ++i, ++location){
            memoryPool[location] = program;
        }
    }
    else{
        printf("I'm sorry, Dave. I can't allocate enough memory for that. %d bytes for program %c is more than I can remember.\n", amount, program);
    }
    /*return nothing;*/
}


/*function to allocate memory with worstFit algorithm*/
void worstFit(char *memoryPool, char program, int amount){
    int beg = 0;
    int end = 0;
    int testBeg = 0;
    int testEnd = 0;
    
    int location = 0;
    for(int i = 0; i < MEMORY_SIZE; ++i){
        if(DEBUG_LEVEL >= 2) printf("We're in the worstFit() for() loop.\n"); fflush(stdout);
        if(memoryPool[i] != '.'){
            if((testEnd - testBeg) >= amount && (testEnd - testBeg) > (end - beg)){
                beg = testBeg;
                end = testEnd;
            }
            testBeg = i + 1; /*looking for a new beginning*/
        }
        testEnd = i; /*testEnd follows i*/
    }
    /*If the memoryPool is free at the end, this test won't automatically run in the for() loop.
     So, we have to run it manually, here.*/
    if((testEnd - testBeg) >= amount && (testEnd - testBeg) > (end - beg)){
        beg = testBeg;
        end = testEnd;
    }
    if(end > 0){  /*We can allocate, if we've changed from the default value of end.*/
        location = beg;
        for(int i = 0; i < amount; ++i, ++location){
            memoryPool[location] = program;
        }
    }
    else{
        printf("I'm sorry, Dave. I can't allocate enough memory for that. %d bytes for program %c is more than I can remember.\n", amount, program);
    }
    /*return nothing;*/
}


/*function to compact the memory*/
void compactMemory(char *memoryPool){
    if(DEBUG_LEVEL >= 2) printf("compacting memory\n");
    /*put code here to compact the memory*/
    int availabeSpace = 0;
    for(int i = 0; memoryPool[i]; ++i){
        if(memoryPool[i] != '.'){
            memoryPool[availabeSpace] = memoryPool[i];
            ++availabeSpace;
        }
    }
    while(availabeSpace < MEMORY_SIZE) memoryPool[availabeSpace++] = '.';
    /*return nothing;*/
}


/*function to free the memory for a program*/
void freeMemory(char *memoryPool, char program){
    if(DEBUG_LEVEL >= 2) printf("Freeing memory for program %c\n", program);
    for(int i = 0; memoryPool[i]; ++i){
        if(memoryPool[i] == program) memoryPool[i] = '.';
    }
    /*return nothing;*/
}
