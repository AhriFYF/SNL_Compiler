%{
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct Token {
    char* value1;
    char* value2;
} Token;

Token* tokens = NULL;      
int token_count = 0;         
FILE* token_file= NULL;   

void save_token(const char* v1, const char* v2); 
void print_all_tokens();

typedef enum
{
    ENDFILE,
    ERROR,
    PROGRAM,
    PROCEDURE,
    TYPE,
    VAR,
    IF,
    THEN,
    ELSE,
    FI,
    WHILE,
    DO,
    ENDWH,
    BEGIN1,
    END1,
    READ,
    WRITE,
    ARRAY,
    OF,
    RECORD,
    RETURN1,
    INTEGER,
    CHAR1,
    ID,
    INTC,
    CHARC,
    ASSIGN,
    EQ,
    LT,
    PLUS,
    MINUS,
    TIMES,
    OVER,
    LPAREN,
    RPAREN,
    DOT,
    COLON,
    SEMI,
    COMMA,
    LMIDPAREN,
    RMIDPAREN,
    UNDERANGE
} LexType;

#define MAXRESERVED 21
#define MAX_STR_LEN 20

static struct
{
    char *str;
    LexType tok;
} reservedWords[MAXRESERVED] = {
    {"program", PROGRAM}, {"type", TYPE}, {"var", VAR}, {"procedure",
    PROCEDURE}, {"begin", BEGIN1}, {"end", END1}, {"array", ARRAY}, 
    {"of", OF}, {"record", RECORD}, {"if", IF}, {"then", THEN}, 
    {"else", ELSE}, {"fi", FI}, {"while", WHILE}, {"do", DO}, 
    {"endwh", ENDWH}, {"read", READ}, {"write", WRITE}, {"return", RETURN1},
    {"integer", INTEGER}, {"char", CHAR1}};

LexType reservedLookup(char *s);
char yylval[20];
int flag = 0;
%}

%option noyywrap
%x COMMENT
DIGIT    [0-9]
ID         [a-zA-Z][a-zA-Z0-9]*
CHAR_CONST    '[a-z,A-Z,0-9]'

%%

"EOF" if (flag == 0) return ENDFILE;

{CHAR_CONST}    { 
                    strncpy(yylval, yytext+1, sizeof(yylval)-1); 
                    yylval[strlen(yylval)-1] = '\0';  
                    return CHARC; 
                }

{ID}            { 
                    if (flag == 0) { 
                        strncpy(yylval, yytext, sizeof(yylval)-1); 
                        LexType t = reservedLookup(yytext);
                        return t;
                    }
                }

{DIGIT}+        { 
                    strncpy(yylval, yytext, sizeof(yylval)-1);
                    return INTC; 
                }

":="    { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return ASSIGN; } }
".."    { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return UNDERANGE; } }

"+"     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return PLUS; } }
"-"     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return MINUS; } }
"*"     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return TIMES; } }
"/"     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return OVER; } }
"("     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return LPAREN; } }
")"     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return RPAREN; } }
"["     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return LMIDPAREN; } }
"]"     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return RMIDPAREN; } }
";"     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return SEMI; } }
":"     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return COLON; } }
","     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return COMMA; } }
"<"     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return LT; } }
"="     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return EQ; } }
"."     { if (flag == 0) { strncpy(yylval, yytext, sizeof(yylval)-1); return DOT; } }

"{"              { BEGIN(COMMENT); }
<COMMENT>"}"     { BEGIN(INITIAL); }
<COMMENT>.|\n    ;  
[\t\n ] ;
.      return ERROR;
%%
LexType reservedLookup(char *s)
{
    for (int i = 0; i < MAXRESERVED; i++)
    {
        if (strcmp(reservedWords[i].str, s) == 0)
        {
            return reservedWords[i].tok;
        }
    }
    return ID;
}

#define MAX_STR_LEN 20

void save_token(const char* v1, const char* v2) {
    if (!v1 || !v2) {
        fprintf(stderr, "Error: Null token values\n");
        return;
    }

    char safe_v1[MAX_STR_LEN] = {0};
    char safe_v2[MAX_STR_LEN] = {0};
    
    strncpy(safe_v1, v1, MAX_STR_LEN - 1);
    safe_v1[MAX_STR_LEN - 1] = '\0';
    
    strncpy(safe_v2, v2, MAX_STR_LEN - 1);
    safe_v2[MAX_STR_LEN - 1] = '\0';

    Token* new_tokens = realloc(tokens, (token_count + 1) * sizeof(Token));
    if (!new_tokens) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    tokens = new_tokens;

    tokens[token_count].value1 = strdup(safe_v1);
    tokens[token_count].value2 = strdup(safe_v2);

    if (!tokens[token_count].value1 || !tokens[token_count].value2) {
        perror("String duplication failed");
        exit(EXIT_FAILURE);
    }

    fprintf(token_file, "%.*s,%.*s\n", 
           MAX_STR_LEN-1, safe_v1,  
           MAX_STR_LEN-1, safe_v2);
    fflush(token_file);

    token_count++;
}


void print_all_tokens() {
    printf("\n======= Tokens List =======\n");
    for (int i = 0; i < token_count; i++) {
        printf("[%02d] %-15s : %s\n", 
              i+1, 
              tokens[i].value1,
              tokens[i].value2);
    }
    printf("==========================\n");
}   


void printToken(int token)
{
    switch (token)
    {
    case ENDFILE:
        save_token("EOF", "EOF");
        break;
    case ERROR:
        printf("\n");
        printf("ERROR\n");
        printf("´Ê·¨·ÖÎö³öÏÖ´íÎó:'%s' \n", yylval);
        break;
    case PROGRAM:
        save_token("reserved word", "PROGRAM");
        break;
    case PROCEDURE:
        save_token("reserved word", "PROCEDURE");
        break;
    case TYPE:
        save_token("reserved word", "TYPE");
        break;
    case VAR:
        save_token("reserved word", "VAR");
        break;
    case IF:
        save_token("reserved word", "IF");
        break;
    case THEN:
        save_token("reserved word", "THEN");
        break;
    case ELSE:
        save_token("reserved word", "ELSE");
        break;
    case FI:
        save_token("reserved word", "FI");
        break;
    case WHILE:
        save_token("reserved word", "WHILE");
        break;
    case DO:
        save_token("reserved word", "DO");
        break;
    case ENDWH:
        save_token("reserved word", "ENDWH");
        break;
    case BEGIN1:
        save_token("reserved word", "BEGIN");
        break;
    case END1:
        save_token("reserved word", "END");
        break;
    case READ:
        save_token("reserved word", "READ");
        break;
    case WRITE:
        save_token("reserved word", "WRITE");
        break;
    case ARRAY:
        save_token("reserved word", "ARRAY");
        break;
    case OF:
        save_token("reserved word", "OF");
        break;
    case RECORD:
        save_token("reserved word", "RECORD");
        break;
    case RETURN1:
        save_token("reserved word", "RETURN");
        break;
    case INTEGER:
        save_token("reserved word", "INTEGER");
        break;
    case CHAR1:
        save_token("reserved word", "CHAR");
        break;
    case ID:
        save_token("ID", yylval);
        break;
    case INTC:
        save_token("INTC", yylval);
        break;
    case CHARC:
        save_token("CHARC", yylval);
        break;
    case ASSIGN:
        save_token("ASSIGN", yylval); 
        break;
    case EQ:
        save_token("EQ", yylval);
        break;
    case LT:
        save_token("LT", yylval);
        break;
    case PLUS:
        save_token("PLUS", yylval);
        break;
    case MINUS:
        save_token("MINUS", yylval);
        break;
    case TIMES:
        save_token("TIMES", yylval);
        break;
    case OVER:
        save_token("OVER", yylval);
        break;
    case LPAREN:
        save_token("LPAREN", yylval);
        break;
    case RPAREN:
        save_token("RPAREN", yylval);
        break;
    case DOT:
        save_token("DOT", yylval); 
        break;
    case LMIDPAREN:
        save_token("LMIDPAREN", yylval);
        break;
    case RMIDPAREN:
        save_token("RMIDPAREN", yylval);
        break;
    case SEMI:
        save_token("SEMI", yylval); 
        break;
    case COLON:
        save_token("COLON", yylval); 
        break;
    case COMMA:
        save_token("COMMA", yylval);
        break;
    case UNDERANGE:
        save_token("UNDERANGE", yylval);
        break;
    }
    return;
}


void main() {
    const char* input_path = "text\\input.txt";
    const char* output_path = "text\\output.txt";

    FILE* input_file = fopen(input_path, "r");
    if (!input_file) {
        fprintf(stderr, "[ERROR] : %s\n", input_path);
        perror("´íÎó");
        exit(EXIT_FAILURE);
    }
    yyin = input_file;

    token_file = fopen(output_path, "w");
    if (!token_file) {
        fclose(input_file); 
        fprintf(stderr, "[ERROR] : %s\n", output_path);
        perror("´íÎó");
        exit(EXIT_FAILURE);
    }

    int n = 1;
    while (n) {
        n = yylex();
        printToken(n);
    }

    fclose(input_file);
    fclose(token_file);

    print_all_tokens();

    for (int i = 0; i < token_count; i++) {
        free(tokens[i].value1);
        free(tokens[i].value2);
    }
    free(tokens);
}