
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdint.h>

#define MAX_IDENTIFIER_SIZE 128
#define MAX_VARIABLE_COUNT 1024
#define MAX_ARGUMENT_COUNT 64
/////////////////////////////////////////////////
/////////////Tokenizer///////////////////////////
////////////////////////////////////////////////

enum TokenKind
{
    TokenKind_Invalid,
    TokenKind_Number,

    TokenKind_Identifier,
    TokenKind_Comma, //,
    TokenKind_OR,// ||
    TokenKind_AND,// &&
    TokenKind_String,
    TokenKind_Equal,//==
    TokenKind_SemiColon,//;
    TokenKind_Plus,//+
    TokenKind_Minus,//-
    TokenKind_Multiply,//*
    TokenKind_Divide,// /
    TokenKind_OpenBrace,// (
    TokenKind_CloseBrace,// )
    TokenKind_BitwiseAND, // &
    TokenKind_BitwiseOR, // |
    TokenKind_BitwiseEqual ,// ==
    TokenKind_BitwiseNot  // ~
};
const char* TokenNames[] = {
    "invalid", "Number", "Identifier", ",", "||", "&&", "String", "==", ";", "+", "-", "*", "/", "(", ")", "&", "|", "==", "~"
};
struct Token
{

    TokenKind Kind;
    int IdentifierLen;
    ReturnValue Data;

};
struct Tokenizer
{
    const char* Input;
    int Position;
    int Length;

};

struct Parser
{
    Tokenizer Lex;
    Token     Current;
    bool      Parsing;
};

Parser StartParsing(const char *str, int length) {
    Parser parser = {};
    parser.Lex.Input = str;
    parser.Lex.Position = 0;
    parser.Lex.Length = length;
    AdvanceToken(&parser);
    return parser;
}


char* ReadEntireFile(const char* filepath)
{
    FILE* fp = fopen(filepath, "rb");
    if (!fp)
    {
        printf("failed to open file: %s\n", filepath);
        exit(-1);
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* input = (char*)malloc(length + 1);
    fread(input, length, 1, fp);
    fclose(fp);

    input[length] = 0; // Null character
    return input;
}


struct Memory
{

    ReturnValue Ans;
    Variable Vars[];
    int VariableCount;
};
void  main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("error: expected file path\n");
        exit(-1);
    }

    const char* filepath = argv[1];
    char* input = ReadEntireFile(filepath);
    Parser parser = StartParsing(input, strlen(input));
    Memory memory;
    memset(&memory, 0, sizeof(memory));
    


}
