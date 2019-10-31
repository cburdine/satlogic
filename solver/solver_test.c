#include <stdio.h>
#include <stdlib.h>
#include "dpll.h"

#define BUFF_SIZE 1024
#define MAX_NUM_CLAUSES 65536

Bool parseFile(const char* file, int* arr, int* numClauses, int maxNumClauses);

int main(int argc, char** argv){

    Bool result;
	int i;
	int* sentence = malloc(MAX_NUM_CLAUSES*3*sizeof(int));
	int numClauses = 0;

	printf("parsing %d files...\n", argc - 1);

	for(i = 1; i < argc; ++i){
		if(!parseFile(argv[i], sentence, &numClauses, MAX_NUM_CLAUSES)){
			printf("unable to parse file: %s\n", argv[i]);
		}else{
			result = dpll3Sat(sentence, numClauses);
			printf("[%s]: %d\n", argv[i], result);
		}
	}

    return 0;
}

Bool parseFile(const char* file, int* arr, int* numClauses, int maxNumClauses){

    FILE* fileIn;
 
	size_t lineBuffSize = BUFF_SIZE;
	int i;
	char* lineBuffer;
	int vars[4];
	int lineNum;
	
	lineBuffer = malloc(lineBuffSize * sizeof(char));
	lineNum = 0;
	*numClauses = 0;

	/* parse cnf file */
	if((fileIn = fopen(file, "r")) != NULL){

		while(getline(&lineBuffer, &lineBuffSize, fileIn) > 0 && *numClauses <= MAX_NUM_CLAUSES){
			++lineNum;
			for(i = 0; i < 4; ++i){ vars[i] = 0; }
			sscanf(lineBuffer, "%d %d %d %d", &vars[0], &vars[1], &vars[2], &vars[3]);

			if(vars[0] && vars[1] && vars[2] && !vars[3]){
				/* printf("V: %d %d %d\n", vars[0], vars[1], vars[2]); */
				arr[3*(*numClauses)] = vars[0];
				arr[3*(*numClauses) + 1] = vars[1];
				arr[3*(*numClauses) + 2] = vars[2];
				++(*numClauses);
			}
		}

		fclose(fileIn);
		free(lineBuffer);

		if(*numClauses <= MAX_NUM_CLAUSES && *numClauses > 0){
			return 1;
		} else {
			if(*numClauses > 0)
				puts("Parser error- buffer size exceeded.\n");
			return 0;
		}
		

	}else{

		free(lineBuffer);
		return 0;
	}

}

