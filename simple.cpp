
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


#define ArrayCount(Array) (sizeof(Array)/sizeof((Array)[0]))
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
    TokenKind_EqualEqual,// ==
    TokenKind_BitwiseNot  // ~
};
const char* TokenNames[] = {
    "invalid", "Number", "Identifier", ",", "||", "&&", "String", "==", ";", "+", "-", "*", "/", "(", ")", "&", "|", "==", "~"
};

enum ReturnKind
{
    ReturnKind_Int,
    ReturnKind_Float,
    Returnkind_Char

};
struct ReturnValue
{
    ReturnKind Kind;
    int Int;
    double Float;
    char Identifier[MAX_IDENTIFIER_SIZE];

};

struct Token
{
    TokenKind Kind;
    int64_t IdentifierLen; // bettter than int because it is perfect for precision but only works for 64 bit ; also int has a different size on 32 bits
    ReturnValue Data;
};
struct Tokenizer
{
    const char *Input;
    int Position;
    int Length;
};

struct Parser
{
    Tokenizer Lex;
    Token     Current;
    bool      Parsing;
};

bool SkipWhiteSpaces(Tokenizer *Tokenizer)
{
    char ch = Tokenizer->Input[Tokenizer->Position];
    if (!isspace(ch))
        return false;
    while (Tokenizer->Position < Tokenizer->Length)
    {
        char ch = Tokenizer->Input[Tokenizer->Position];
        if (!isspace(ch))
            break;
        ++(*Tokenizer).Position;// Tokenizer->Positon+=1;
    }
    return true;
}

const char SingleCharTokenInput[] = {
 ',', '||', '&&', '==', ';', '+', '-', '*', '/', '(', ')', '&', '|', '==', '~'
};

const TokenKind SingleCharTokenOutput[] =
{
    TokenKind_Comma, //,
    TokenKind_OR,// ||
    TokenKind_AND,// &&
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
    TokenKind_EqualEqual ,// ==
    TokenKind_BitwiseNot  // ~
};

struct DoubleCharToken {
    char A, B;
};

const DoubleCharToken DoubleCharTokenInput[] = { {'=', '='}, { '|', '|'}, { '&', '&' } };
const TokenKind   DoubleCharTokenOutput[] = 
{
    TokenKind_EqualEqual, TokenKind_BitwiseOR, TokenKind_BitwiseAND
};

bool IsNum(char n)
{
    return n >= '0' && n <= '9';
}

bool IsNumberSeparator(char c)
{
    for (int iter = 0; iter < ArrayCount(SingleCharTokenInput); ++iter) 
    {
        if (c == SingleCharTokenInput[iter])
            return true;
    }
    for (int iter = 0; iter < ArrayCount(DoubleCharTokenInput); ++iter)
    {
        if (c == DoubleCharTokenInput[iter].A)
            return true;
    }
    return isspace(c);
}

bool Tokenize(Tokenizer* Tokenizer, Token* Token)
{
    memset(Token, 0, sizeof(*Token));// initialize the token structure first
    while (Tokenizer->Position < Tokenizer->Length)
    {
        ///skip all the whitespaces until a valid identifier is found
        if (!SkipWhiteSpaces(Tokenizer))
        {
            break;
        }
    }
    if (Tokenizer->Position >= Tokenizer->Length)
        return false;
    char a = Tokenizer->Input[Tokenizer->Position];
    for (int i = 0;i < ArrayCount(DoubleCharTokenInput);i++)
    {
        if (Tokenizer->Position + 1 < Tokenizer->Length)
        {
            char b = Tokenizer->Input[Tokenizer->Position + 1];
            if ((a == DoubleCharTokenInput[i].A) && (b == DoubleCharTokenInput[i].B))
            {
                Token->Kind = DoubleCharTokenOutput[i];
                Tokenizer->Position += 2;
                return true;
            }
        }

        if (a == SingleCharTokenInput[i])
        {
            Token->Kind = SingleCharTokenOutput[i];
            Tokenizer->Position += 1;
            return true;
        }
    }

    if (IsNum(a))
    {
        char str[MAX_IDENTIFIER_SIZE] = {};
        int len = 0;
        while (Tokenizer->Position < Tokenizer->Length)
        {
            a = Tokenizer->Input[Tokenizer->Position];
            if (IsNumberSeparator(a)) break;
            str[len] = a;
            len++;
            Tokenizer->Position++;

            if (len == MAX_IDENTIFIER_SIZE) {
                printf("too long number\n");
                exit(1);
            }
        }
        char* endptr1 = 0;
        char* endptr2 = 0;
        ReturnValue number;
        number.Float = strtod(str, &endptr1); //parse string to double 
        number.Int = strtol(str, &endptr2, 10);// parse string to long int
        number.Kind = endptr1 == endptr2 ? ReturnKind_Int : ReturnKind_Float;

        if (str + len != endptr1)
        {
            printf("invalid number\n");
            exit(1);
        }

        Token->Kind = TokenKind_Number;
        Token->Data = number;

        return true;
    }
    if (isalnum(a) || a == '_')
    {
        while (Tokenizer->Position < Tokenizer->Length)
        {
            a = Tokenizer->Input[Tokenizer->Position];
            if (!isalnum(a) && a != '_')
            {
                break;
            }
            Token->Data.Identifier[Token->IdentifierLen] = a;
            Token->IdentifierLen += 1;
            Tokenizer->Position += 1;

            if (Token->IdentifierLen == MAX_IDENTIFIER_SIZE)
            {
                printf("too long identifier\n");
                exit(1);
            }
            return true;
        }
        printf("invalid character: %c\n", a);
        exit(1);
    
        return false;

    }
}



void AdvanceToken(Parser* parser) 
{ /// the main function which will forward the parsing of the token
    parser->Parsing = Tokenize(&parser->Lex, &parser->Current);

}

Parser StartParsing(const char *str, int length) {
    Parser parser = {};
    parser.Lex.Input = str;
    parser.Lex.Position = 0;
    parser.Lex.Length = length;
    AdvanceToken(&parser);
    return parser;
}


char *ReadFile(const char* filepath)
{
    FILE* fp;
    fopen_s(&fp, filepath, "rb");
    if (!fp)
    {
        printf("failed to open file: %s\n", filepath);
        exit(-1);
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* input = (char*)malloc(length + 1);
    if (!input)
    {
        printf("resource isnot enough for processing\n");
        exit(-1);
    }
    fread(input, length, 1, fp);
    fclose(fp);

    input[length] = 0; // Null character
    return input;
}


////////////////////////////////////////////
//////////////Evaluate////////////////////////
////////////////////////////////////////

struct Variable
{
    char Name[MAX_IDENTIFIER_SIZE];
    ReturnValue Value;
};

struct Memory
{
    ReturnValue Ans;
    Variable Vars[MAX_VARIABLE_COUNT];
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
    char* input = ReadFile(filepath);
    Parser parser = StartParsing(input, strlen(input));
    printf("Token is %s\n", parser.Current.Data);
    Memory memory;
    memset(&memory, 0, sizeof(memory));
    



}
