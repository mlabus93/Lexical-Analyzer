//
//  main.c
//  LexicalAnalyzer
//
//  Created by Michael Labus on 6/8/15.
//  Copyright (c) 2015 Michael Labus. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#define IDENTIFIER_MAX_LENGTH 11
#define NUMBER_MAX_LENGTH 5
#define DEFAULT_SIZE 500

char* ReadFile(FILE* input, int fileSize, char *code);
void CheckForComment(int fileSize, FILE* cleanInput);

typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym
} token_type;


char *code, *cleanCode;

int main(int argc, const char * argv[]) {
    
    // File pointers
    FILE *input, *cleanInput, *lexemeTable, *lexemeList;
    
    int fileSize, cleanFileSize;
    
    // Open input file
    input = fopen("/Users/michaellabus/Documents/School/Summer2015/SystemsSoftware/LexicalAnalyzer/LexicalAnalyzer/input.txt", "r");
    
    // Read to file end for length of file
    fseek(input, 0L, SEEK_END);
    fileSize = ftell(input) + 1;
    
    // Allocate memory for size of file
    code = malloc(fileSize*sizeof(char));
    
    // Error checking
    if (code == NULL) {
        printf("Error allocatiing memory.\n");
        return 1;
    }
    
    // Reset pointer to beginning of file for scanning
    fseek(input, 0L, SEEK_SET);
    
    // Debugging code
    printf("%d\n", fileSize);
    
    // Read file into array
    ReadFile(input, fileSize, code);
    
    // Close file after use
    fclose(input);
    
    // Debugging code
    printf("\n\n");
    
    // Open file to write to without comments
    cleanInput = fopen("/Users/michaellabus/Documents/School/Summer2015/SystemsSoftware/LexicalAnalyzer/LexicalAnalyzer/cleaninput.txt", "w");
    
    //  Output cleanInput file that removes comments
    CheckForComment(fileSize, cleanInput);
    
    // Close file after use
    fclose(cleanInput);
    
    // Open clean input for reading
    cleanInput = fopen("/Users/michaellabus/Documents/School/Summer2015/SystemsSoftware/LexicalAnalyzer/LexicalAnalyzer/cleaninput.txt", "r");
    
    // Read to file end for length
    fseek(cleanInput, 0L, SEEK_END);
    cleanFileSize = ftell(cleanInput) + 1;
    
    // Allocate memory for size of file
    cleanCode = malloc(cleanFileSize*sizeof(char));
    
    // Error checking
    if (cleanCode == NULL) {
        printf("Error allocating memory.\n");
        return 1;
    }
    
    // Reset pointer to beginning of file for scanning
    fseek(cleanInput, 0L, SEEK_SET);
    
    // Read file into array
    ReadFile(cleanInput, cleanFileSize, cleanCode);
    
    // Free memory of allocated arrays
    free(code);
    free(cleanCode);
    
    return 0;
}


char* ReadFile(FILE* input, int fileSize, char *code) {
    int i = 0;
    while (i < fileSize) {
        char c;
        c = fgetc(input);
        code[i] = (char)c;
        if (code[i] != '\377') {
            printf("%c", code[i]);
        }
        i++;
    }
    return code;
}

void CheckForComment(int fileSize, FILE* cleanInput)
{
    int state = 1;
    char c;
    for (int i=0; i<fileSize; i++) {
        c = code[i];
        switch (state) {
            case 1:
                if (c == '/') {
                    state = 2;
                }
                else {
                    fprintf(cleanInput, "%c", c);
                }
                break;
            case 2:
                if (c == '*') {
                    state = 3;
                }
                else {
                    fprintf(cleanInput, "/%c", c);
                    state = 1;
                }
                break;
            case 3:
                if (c == '*') {
                    state = 4;
                }
                break;
            case 4:
                if (c == '/') {
                    state = 5;
                }
                else if (c != '*') {
                    state = 3;
                }
                break;
            case 5:
                fprintf(cleanInput, "%c", c);
                state = 1;
                break;
        }
    }
}






