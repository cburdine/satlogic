#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dpll.h"
#include "test/test.h"

#define BUFF_SIZE 1024
#define MAX_NUM_CLAUSES 65536

Bool runTests();
int abs3Max(int a, int b, int c);
Bool parseFile(const char* file, int* arr, int* numClauses, int* maxVar, int maxNumClauses);

int main(int argc, char** argv){

	if(argc > 1 && (strcmp(argv[1],"-t") == 0 || strcmp(argv[1],"--test") == 0)){
		/* perform tests first */
		runTests();
		return 0;
	}

    Bool result;
	int i,j;
	int* sentence = malloc(MAX_NUM_CLAUSES * CLAUSE_SIZE * sizeof(int));
	Bool* solution = malloc(MAX_NUM_CLAUSES * CLAUSE_SIZE * sizeof(Bool));
	int numClauses = 0;
	int maxVar = 0;
	
	printf("parsing %d files...\n", argc - 1);

	for(i = 1; i < argc; ++i){
		if(!parseFile(argv[i], sentence, &numClauses, &maxVar, MAX_NUM_CLAUSES)){
			printf("unable to parse file: %s\n", argv[i]);

		}else{
			result = dpll3Sat(sentence, numClauses, solution);
			if(result){
				printf("[%s]:", argv[i]);
				for(j = 1; j <= maxVar; ++j){
					printf(" %d=%s", j, (solution[j]? "T" : "F"));
				}
				putchar('\n');
			} else {
				printf("[%s]: UNSATISFIABLE\n", argv[i]);
			}
		}
	}

	free(sentence);
	free(solution);

    return 0;
}

Bool parseFile(const char* file, int* arr, int* numClauses, int* maxVar, int maxNumClauses){

    FILE* fileIn;
 
	size_t lineBuffSize = BUFF_SIZE;
	int i, clauseVarMax;
	char* lineBuffer;
	int vars[4];
	int lineNum;
	
	lineBuffer = malloc(lineBuffSize * sizeof(char));
	lineNum = 0;
	*numClauses = 0;
	*maxVar = 0;
	clauseVarMax = 0;

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

				clauseVarMax = abs3Max(vars[0], vars[1], vars[2]);
				if(clauseVarMax > *maxVar){ *maxVar = clauseVarMax; }

				++(*numClauses);
			}
		}

		fclose(fileIn);
		free(lineBuffer);

		if(*numClauses <= MAX_NUM_CLAUSES && *numClauses > 0 && *maxVar <= MAX_NUM_CLAUSES*3){
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

int abs3Max(int a, int b, int c){
	int max;

	a = abs(a);
	b = abs(b);
	c = abs(c);

	max = ((a > b)? a : b);
	return (max > c)? max : c;
}

Bool runTests(){
	int passed;
	puts("Running tests...\n");
	passed = runManifestTestGroups(stdout);
	return passed;
}

