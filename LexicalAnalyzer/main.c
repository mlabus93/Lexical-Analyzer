//
//  main.c
//  LexicalAnalyzer
//
//  Created by Michael Labus and Ryan Gonyon on 6/8/15.
//  Copyright (c) 2015 Michael Labus. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IDENTIFIER_MAX_LENGTH 11
#define NUMBER_MAX_LENGTH 5
#define DEFAULT_SIZE 500

char* ReadFile(FILE* input, int fileSize, char *code);
void CheckForComment(int fileSize, FILE* cleanInput);
int Tokens(int cleanFileSize, int *i, int j);
int TempStringFunc(char tempString[], int j);
void GetTokenType(int j);
void ErrorHandling(FILE* lexemeTable, FILE* lexemeList, int error);

typedef enum {
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
    multsym = 6,  slashsym = 7, oddsym = 8, eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
    periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
    whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
    readsym = 32, elsesym = 33
} token_type;

typedef char* string;

typedef struct {
    char lexeme[IDENTIFIER_MAX_LENGTH];
    int tokenType;
}tokenTable;

char *code, *cleanCode;
string reservedWords[14];
char specialSymbols[13];
tokenTable *table;
int error = 0;
string tempStringError;

// File pointers
FILE *input, *cleanInput, *lexemeTable, *lexemeList;

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
    
    

    
    // File sizes
    int fileSize, cleanFileSize;
    
    // Open input file
    input = fopen("/Users/michaellabus/Documents/School/Summer2015/SystemsSoftware/LexicalAnalyzer/LexicalAnalyzer/input.txt", "r");
    //input = fopen(argv[1], "r");
    
    // Open file to write to without comments
    cleanInput = fopen("/Users/michaellabus/Documents/School/Summer2015/SystemsSoftware/LexicalAnalyzer/LexicalAnalyzer/cleaninput.txt", "w");
    //cleanInput = fopen(argv[2], "w");
    
    // Open files for writing
    lexemeTable = fopen("/Users/michaellabus/Documents/School/Summer2015/SystemsSoftware/LexicalAnalyzer/LexicalAnalyzer/lexemetable.txt", "w");
    //lexemeTable = fopen(argv[3], "w");
    lexemeList = fopen("/Users/michaellabus/Documents/School/Summer2015/SystemsSoftware/LexicalAnalyzer/LexicalAnalyzer/lexemeList.txt", "w");
    //lexemeList = fopen(argv[4], "w");
    
    
    // Read to file end for length of file
    fseek(input, 0L, SEEK_END);
    fileSize = (int)ftell(input) + 1;
    
    // Allocate memory for size of file
    code = malloc(fileSize*sizeof(char));
    
    // Error checking
    if (code == NULL) {
        printf("Error allocatiing memory.\n");
        return 1;
    }
    
    // Reset pointer to beginning of file for scanning
    fseek(input, 0L, SEEK_SET);
    
    // Read file into array
    ReadFile(input, fileSize, code);
    
    // Close file after use
    fclose(input);
    
    // Debugging code
    printf("\n\n");
    
    
    //  Output cleanInput file that removes comments
    CheckForComment(fileSize, cleanInput);
    
    // Close file after use
    fclose(cleanInput);
    
    // Open clean input for reading
    cleanInput = fopen("/Users/michaellabus/Documents/School/Summer2015/SystemsSoftware/LexicalAnalyzer/LexicalAnalyzer/cleaninput.txt", "r");
    //cleanInput = fopen(argv[2], "r");
    
    // Read to file end for length
    fseek(cleanInput, 0L, SEEK_END);
    cleanFileSize = (int)ftell(cleanInput) + 1;
    
    // Allocate memory for size of file
    cleanCode = malloc(cleanFileSize*sizeof(char));
    
    // Error checking
    if (cleanCode == NULL) {
        printf("Error allocating memory.\n");
        return 1;
    }
    
    // Allocate memory for token table
    table = malloc(cleanFileSize*sizeof(tokenTable));
    
    // Error checking
    if (table == NULL) {
        printf("Error allocating memory.\n");
    }
    
    // Reset pointer to beginning of file for scanning
    fseek(cleanInput, 0L, SEEK_SET);
    
    // Read file into array
    ReadFile(cleanInput, cleanFileSize, cleanCode);
    
    // Read tokens
    int i = 0, j = 0;
    printf("\n");
    while (i < cleanFileSize) {
        j = Tokens(cleanFileSize, &i, j);
        i++;
    }
    
    // Get token syms
    GetTokenType(j);
    
    // Write to lexeme table file
    fprintf(lexemeTable, "lexeme    token type\n");
    for (int k=0; k<j; k++) {
        fprintf(lexemeTable, "%s", table[k].lexeme);
        fprintf(lexemeTable, "%10d", table[k].tokenType);
        fprintf(lexemeTable, "\n");
    }
    
    // Write to lexeme list
    for (int k=0; k<j; k++) {
        if (table[k].tokenType == 2) {
            fprintf(lexemeList, "%d %s ", table[k].tokenType, table[k].lexeme);
        }
        else if (table[k].tokenType == 3) {
            fprintf(lexemeList, "%d %s ", table[k].tokenType, table[k].lexeme);
        }
        else {
            fprintf(lexemeList, "%d ", table[k].tokenType);
        }
    }
    
    // Close files
    fclose(cleanInput);
    fclose(lexemeTable);
    fclose(lexemeList);
    
    // Console output to mimic files
    printf("LEXEME TABLE\n");
    printf("lexeme    token type\n");
    for (int k=0; k<j; k++) {
        printf("%s", table[k].lexeme);
        printf("%10d", table[k].tokenType);
        printf("\n");
    }
    
    printf("\n");
    printf("LEXEME LIST\n");
    for (int k=0; k<j; k++) {
        if (table[k].tokenType == 2) {
            printf("%d %s ", table[k].tokenType, table[k].lexeme);
        }
        else if (table[k].tokenType == 3) {
            printf("%d %s ", table[k].tokenType, table[k].lexeme);
        }
        else {
            printf("%d ", table[k].tokenType);
        }
    }

    
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

int Tokens(int cleanFileSize, int *i, int j) {
    //printf("\n");
    char tempString[IDENTIFIER_MAX_LENGTH+1] = "";
    char ch = cleanCode[*i];
    while (ch != ' ' && ch != '\0' && ch != '\n' && ch != '\377' && ch != '\r' && ch != '\t') {
        if (ch == specialSymbols[0] || ch == specialSymbols[1] || ch == specialSymbols[2] ||
            ch == specialSymbols[3] || ch == specialSymbols[4] || ch == specialSymbols[5] ||
            ch == specialSymbols[7] || ch == specialSymbols[8] || ch == specialSymbols[11]) {
            table[j].lexeme[0] = ch;
            j++;
            //TempStringFunc(tempString);
        }
        else if (ch == specialSymbols[9] || ch == specialSymbols[10] || ch == specialSymbols[12]) {
            table[j].lexeme[0] = ch;
            (*i)++;
            char nextCh = (char)cleanCode[*i];
            if (ch == specialSymbols[9]) {
                switch (nextCh) {
                    case '=':
                        table[j].lexeme[1] = nextCh;
                        break;
                    case '>':
                        table[j].lexeme[1] = nextCh;
                        break;
                    default:
                        // do nothing
                        break;
                }
            }
            if (ch == specialSymbols[10]) {
                switch (nextCh) {
                    case '=':
                        table[j].lexeme[1] = nextCh;
                        break;
                    default:
                        // do nothing
                        break;
                }
            }
            if (ch == specialSymbols[12]) {
                switch (nextCh) {
                    case '=':
                        table[j].lexeme[1] = nextCh;
                        break;
                    default:
                        error = 4;
                        ErrorHandling(lexemeTable, lexemeList, error);
                        printf("ERROR: Invalid symbol. %c is not a symbol.\n", specialSymbols[12]);
                        break;
                }
            }
            j++;
        }
        else {
            int length = (int)strlen(tempString);
            if (length < 11) {
                tempString[length] =  ch;
                tempString[length+1] = '\0';
                int nextIndex = *i + 1;
                char nextChar = cleanCode[nextIndex];
                if ( !( (nextChar >= '0' && nextChar <= '9') || (nextChar >= 'a' && nextChar <= 'z') ||
                        (nextChar >= 'A' && nextChar <= 'Z') ) ) {
                    break;
                }
            }
            else {
                error = 1;
                tempStringError = tempString;
                ErrorHandling(lexemeTable, lexemeList, error);
                printf("\nERROR: Variable name %s<val> is too long.\n", tempString);
                return j;
            }
        }
        (*i)++;
        ch = cleanCode[*i];
    }
    if (!(strcmp(tempString, "") == 0)) {
        j = TempStringFunc(tempString, j);
    }
    return j;
}

int TempStringFunc(char tempString[], int j) {
    int flag = 0;
    if (tempString[0] >= '0' && tempString[0] <= '9') {
        if ((tempString[1] >= 'a' && tempString[1] <= 'z') ||
            (tempString[1] >= 'A' && tempString[1] <= 'Z')) {
            error = 2;
            tempStringError = tempString;
            ErrorHandling(lexemeTable, lexemeList, error);
            printf("ERROR: Variable %s<val> can't start with letter.\n", tempString);
            return 0;
        }
        for (int i=1; i<strlen(tempString); i++) {
            if (!(tempString[i] >= '0' && tempString[i] <= '9')) {
                flag = 1;
                error = 3;
                ErrorHandling(lexemeTable, lexemeList, error);
                printf("ERROR: Number can't contain other characters.\n");
                return 0;
            }
        }
        if (flag == 0) {
            for (int i=0; i<strlen(tempString); i++) {
                table[j].lexeme[i] = tempString[i];
            }
            table[j].tokenType = numbersym;
            j++;
        }
    }
    else {
        for (int i=0; i<strlen(tempString); i++) {
            table[j].lexeme[i] = tempString[i];
        }
        j++;
    }
    return j;
}

void GetTokenType(int j) {
    printf("\n\n");
    for (int i=0; i<j; i++) {
        // const
        if (strcmp(table[i].lexeme, reservedWords[0]) == 0) {
            table[i].tokenType = constsym;
        }
        
        // var
        else if (strcmp(table[i].lexeme, reservedWords[1]) == 0) {
            table[i].tokenType = varsym;
        }
        
        // procedure
        else if (strcmp(table[i].lexeme, reservedWords[2]) == 0) {
            table[i].tokenType = procsym;
        }
        
        // call
        else if (strcmp(table[i].lexeme, reservedWords[3]) == 0) {
            table[i].tokenType = callsym;
        }
        
        // begin
        else if (strcmp(table[i].lexeme, reservedWords[4]) == 0) {
            table[i].tokenType = beginsym;
            //printf("%s : %d\n", table[i].lexeme, table[i].tokenType);
        }
        
        // end
        else if (strcmp(table[i].lexeme, reservedWords[5]) == 0) {
            table[i].tokenType = endsym;
        }
        
        // if
        else if (strcmp(table[i].lexeme, reservedWords[6]) == 0) {
            table[i].tokenType = ifsym;
        }
        
        // then
        else if (strcmp(table[i].lexeme, reservedWords[7]) == 0) {
            table[i].tokenType = thensym;
        }
        
        // else
        else if (strcmp(table[i].lexeme, reservedWords[8]) == 0) {
            table[i].tokenType = elsesym;
        }
        
        // while
        else if (strcmp(table[i].lexeme, reservedWords[9]) == 0) {
            table[i].tokenType = whilesym;
        }
        
        // do
        else if (strcmp(table[i].lexeme, reservedWords[10]) == 0) {
            table[i].tokenType = dosym;
        }
        
        // read
        else if (strcmp(table[i].lexeme, reservedWords[11]) == 0) {
            table[i].tokenType = readsym;
        }
        
        // write
        else if (strcmp(table[i].lexeme, reservedWords[12]) == 0) {
            table[i].tokenType = writesym;
        }
        
        // odd
        else if (strcmp(table[i].lexeme, reservedWords[13]) == 0) {
            table[i].tokenType = oddsym;
        }
        
        // +
        else if (strcmp(table[i].lexeme, "+") == 0) {
            table[i].tokenType = plussym;
        }
        
        // -
        else if (strcmp(table[i].lexeme, "-") == 0) {
            table[i].tokenType = minussym;
        }
        
        // *
        else if (strcmp(table[i].lexeme, "*") == 0) {
            table[i].tokenType = multsym;
        }
        
        // /
        else if (strcmp(table[i].lexeme, "/") == 0) {
            table[i].tokenType = slashsym;
        }
        
        // (
        else if (strcmp(table[i].lexeme, "(") == 0) {
            table[i].tokenType = lparentsym;
        }
        
        // )
        else if (strcmp(table[i].lexeme, ")") == 0) {
            table[i].tokenType = rparentsym;
        }
        
        // =
        else if (strcmp(table[i].lexeme, "=") == 0) {
            table[i].tokenType = eqlsym;
        }
        
        // ,
        else if (strcmp(table[i].lexeme, ",") == 0) {
            table[i].tokenType = commasym;
        }
        
        // .
        else if (strcmp(table[i].lexeme, ".") == 0) {
            table[i].tokenType = periodsym;
        }
        
        // <
        else if (strcmp(table[i].lexeme, "<") == 0) {
            table[i].tokenType = lessym;
        }
        
        // >
        else if (strcmp(table[i].lexeme, ">") == 0) {
            table[i].tokenType = gtrsym;
        }
        
        // ;
        else if (strcmp(table[i].lexeme, ";") == 0) {
            table[i].tokenType = semicolonsym;
        }
        
        // :=
        else if (strcmp(table[i].lexeme, ":=") == 0) {
            table[i].tokenType = becomessym;
        }
        
        // <=
        else if (strcmp(table[i].lexeme, "<=") == 0) {
            table[i].tokenType = leqsym;
        }
        
        // >=
        else if (strcmp(table[i].lexeme, ">=") == 0) {
            table[i].tokenType = geqsym;
        }
        
        // <>
        else if (strcmp(table[i].lexeme, "<>") == 0) {
            table[i].tokenType = neqsym;
        }
        
        // ident
        else if (!(table[i].lexeme[0] >= '0' && table[i].lexeme[0] <= '9')) {
            table[i].tokenType = identsym;
        }
    }
}

void ErrorHandling(FILE* lexemeTable, FILE* lexemeList, int error) {
    switch (error) {
        case 0:
            fprintf(lexemeTable, "No Errors.\n");
            fprintf(lexemeList, "No Errors.\n");
            break;
        case 1:
            fprintf(lexemeTable, "ERROR: Variable name %s<val> is too long.\n\n", tempStringError);
            fprintf(lexemeList, "ERROR: Variable name %s<val> is too long.\n\n", tempStringError);
            break;
        case 2:
            fprintf(lexemeTable, "ERROR: Variable %s<val> can't start with letter.\n", tempStringError);
            fprintf(lexemeList, "ERROR: Variable %s<val> can't start with letter.\n", tempStringError);
            break;
        case 3:
            fprintf(lexemeTable, "ERROR: Number can't contain other characters.\n\n");
            fprintf(lexemeList, "ERROR: Number can't contain other characters.\n\n");
            break;
        case 4:
            fprintf(lexemeTable, "ERROR: Invalid symbol. %c is not a symbol.\n\n", specialSymbols[12]);
            fprintf(lexemeList, "ERROR: Invalid symbol. %c is not a symbol.\n\n", specialSymbols[12]);
            break;
    }
}

