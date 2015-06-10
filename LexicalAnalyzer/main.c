//
//  main.c
//  LexicalAnalyzer
//
//  Created by Michael Labus on 6/8/15.
//  Copyright (c) 2015 Michael Labus. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define IDENTIFIER_MAX_LENGTH 11
#define NUMBER_MAX_LENGTH 5
#define DEFAULT_SIZE 500

char* ReadFile(FILE* input, int fileSize, char *code);
void CheckForComment(int fileSize, FILE* cleanInput);
void ReadTokens(int cleanFileSize);
void Tokens(int cleanFileSize);

typedef enum {
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
    multsym = 6,  slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
    periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
    whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
    readsym = 32, elsesym = 33
} token_type;

typedef char* string;

typedef struct {
    string lexeme;
    int tokenType;
}tokenTable;

char *code, *cleanCode;
string reservedWords[14];
char specialSymbols[13];


int main(int argc, const char * argv[]) {
    
    reservedWords[0] = "const";
    reservedWords[1] = "var";
    reservedWords[2] = "procedure";
    reservedWords[3] = "call";
    reservedWords[4] = "begin";
    reservedWords[5] = "end";
    reservedWords[6] = "if";
    reservedWords[7] = "then";
    reservedWords[8] = "else";
    reservedWords[9] = "while";
    reservedWords[10] = "do";
    reservedWords[11] = "read";
    reservedWords[12] = "write";
    reservedWords[13] = "odd";
    
    specialSymbols[0] = '+';
    specialSymbols[1] = '-';
    specialSymbols[2] = '*';
    specialSymbols[3] = '/';
    specialSymbols[4] = '(';
    specialSymbols[5] = ')';
    specialSymbols[6] = '=';
    specialSymbols[7] = ',';
    specialSymbols[8] = '.';
    specialSymbols[9] = '<';
    specialSymbols[10] = '>';
    specialSymbols[11] = ';';
    specialSymbols[12] = ':';
    
    // File pointers
    FILE *input, *cleanInput, *lexemeTable, *lexemeList;
    
    // File sizes
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
    
    // Read tokens
    Tokens(cleanFileSize);
    //ReadTokens(cleanFileSize);
    
    // Free memory of allocated arrays
    free(code);
    free(cleanCode);
    
    return 0;
}

// Function to read files into arrays
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

// Return clean input file without comments
void CheckForComment(int fileSize, FILE* cleanInput) {
    int state = 1;
    char c;
    for (int i=0; i<fileSize-1; i++) {
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

void ReadTokens(int cleanFileSize) {
    char ch;
    int savedIndex;
    for (int i=0; i<cleanFileSize; i++)
    {
        ch = cleanCode[i];
        
        if (ch == '\n' | ch == '\t' | ch == ' ' | ch == '\377') {
            break;
            savedIndex = i + 1;
        }
        else {
        }
    }
}

void Tokens(int cleanFileSize) {
    printf("\n");
    int i = 0;
    char tempString[IDENTIFIER_MAX_LENGTH+1] = "";
    char ch = cleanCode[i];
    while (ch != ' ' && ch != '\0' && ch != '\n') {
        int length = strlen(tempString);
        tempString[length] =  ch;
        tempString[length+1] = '\0';
        i++;
        ch = cleanCode[i];
    }
    printf("%s", tempString);
}





