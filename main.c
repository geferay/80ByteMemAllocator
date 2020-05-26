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
#define INPUT_MAX 40
/*We got to have a version that's +1, for the null terminator.*/
#define INPUT_LEN 41
#define MEMORY_SIZE 80

/*Function Prototypes*/
void compactMemory(char *memoryPool);
void freeMemory(char *memoryPool, char program);


/*Main function*/
/*We don't worry about input. It will just be ignored.*/
int main(){
	printf("Welcome to the 80ByteMemAllocator Demo program, an assignment for CSS 430 (Operating Systems) at UWB.\n");
	printf("You are here, I guess.\n");
	
	const char *memoryTemplate = "AAAAAAAAAA..........BBBBBBBBBBFFFFFFFFFFFFFFFGGG..CCCCCH..............DDDDD.....";
	char *memoryPool;
	memoryPool = (char*)malloc(INPUT_LEN * sizeof(char));
	memoryPool[INPUT_MAX] = '\0';
	for(int i = 0; memoryTemplate[i]; ++i) memoryPool[i] = memoryTemplate[i];
	
	
	//memoryPool[80] = '\0'; /*set the 81st character (81-1) to the null character, because it's the end of the string*/
	
	/*main program loop code*/
	int notDone = 1; /*We're never done, unless somebody implements an exit command or hits Ctr-C*/
	while(notDone){
		/*Get ready for input. We need to do a command prompt with switch() statement.*/
		size_t inputMax   = INPUT_MAX;
		size_t charInput  =  0;        /*getline() tells us the input size*/
		char   input[INPUT_LEN];   /*Way too much space. The likely max needed is 9.*/
		char   command    = '\0';
		char   program    = '\0';
		int    amount     =    0;
		char   algorithm  = '\0';
		char*  parsed     = NULL;
		int    curr       = 0;         /*current position in input string*/
		int    errorFound = 0;
		
		/*getInput*/
		printf("Give a command: ");
		char *in = input; /*ugly hack to get getline() to be happy*/
		charInput = getline(&in, &inputMax, stdin);
		if(charInput <= 1) continue; /*nothing to do here, just repeat the loop*/
		/*remove trailing newline*/
		if(input[charInput - 1] == 10) input[charInput - 1] = '\0';
		/*skip leading spaces*/
		while(input[curr] == ' ') ++curr;
		/*assign command*/
		command = input[curr++];
		
		/*parsing section===============================================================================*/
		if(command == 'R' || command == 'r'){
			command = 'R';
			/*error on extra non-space characters*/
			if(input[curr] != ' ') errorFound = 1;
			if(!errorFound){
				/*skip space characters*/
				/*read the commands from a file*/
			}
		}
		else if(command == 'S' || command == 's'){
			command = 'S';
		}
		else if(command == 'C' || command == 'c'){
			command = 'C';
			/*compact the memory*/
		}
		else if(command == 'F' || command == 'f'){
			command = 'F';
			/*error on extra non-space characters*/
			if(input[curr] != ' ') errorFound = 1;
			if(!errorFound){
				/*skip spaces*/
				while(input[curr] == ' ') ++curr;
				/*assign program*/
				program = input[curr];
				/*make uppercase*/
				if(program > 90) program -= 32;
			}
		}
		else if(command == 'A' || command == 'a'){
			command = 'A';
			/*assign program*/
			program = input[curr];
			/*error on extra non-space*/
			
			/*get integer from text, for space size to allocate*/
			
			/*error on non-space after integer*/
			
			/*assign algorithm*/
			algorithm = input[curr++];
			
		}
		else{
			errorFound = 1; /*There must have been an error.*/
		}
		
		
		printf("Command is: %c\n", command);
		
		if(errorFound == 1){
			/*show error*/
			printf("We're dearly sorry, but your command was not recognized.\n");
			printf("The unrecognized command was: %s\n", input);
			printf("Please check your sytax and try again.\n");
			errorFound = 0; /*leaving this set will confuse the program*/
		}
		else{
			/*run commands*/
			switch(command){
				case 'R':{
					/*setup reading from the file*/
					/*run commands from file*/
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
					freeMemory((char*)memoryPool, program);
				} break;
				case 'A':{
					/*setup reading from the file*/
					/*run commands from file*/
				} break;
				default:{
					/*This is a previously uncaught error.*/
					printf("Something strange has happened, and the request cannot be fulfilled. Perhaps there was something wrong with the syntax, or maybe something more unknown. Please check to make sure everything is in order, and try again.\n");
				} break;
			}
		}
	}
	
	if(memoryPool){
		free(memoryPool);
		memoryPool = NULL;
	}
	return 0; /*Every day's a good day! :-) */
}



/*function to compact the memory*/
void compactMemory(char *memoryPool){
	printf("compacting memory\n");
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
	printf("Freeing memory for program %c\n", program);
	for(int i = 0; memoryPool[i]; ++i){
		if(memoryPool[i] == program) memoryPool[i] = '.';
	}
	/*return nothing;*/
}
