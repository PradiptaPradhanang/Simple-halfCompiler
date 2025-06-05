#pragma once
#include "Token.h"

const char* TokenNames[] = {
	"invalid", "Number", "Identifier", ",", "||", "&&", "String", "==", ";", "+", "-", "*", "/", "(", "{", ")", "}", "&", "|", "==", "~", "return", "func"
};

bool SkipWhiteSpaces(Tokenizer* Tokenizer)
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
	memset(Token, 0, sizeof(*Token));

	while (Tokenizer->Position < Tokenizer->Length)
	{   
		if (!SkipWhiteSpaces(Tokenizer))
		{
			break;
		}
	}
	if (Tokenizer->Position >= Tokenizer->Length)
		return false;

	unsigned char a = Tokenizer->Input[Tokenizer->Position];
	for (int i = 0;i < ArrayCount(DoubleCharTokenInput);i++) // for inputs of operands like && and so on.
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
	}

	for (int i = 0; i < ArrayCount(SingleCharTokenInput); ++i)
	{
		//printf("%d; %c == %c\n", i, a, SingleCharTokenInput[i]);
		if (a == SingleCharTokenInput[i])
		{
			//printf("single character token: %c\n", a);

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
		Value number;
		number.Float = strtod(str, &endptr1); //parse string to double 
		number.Int = strtol(str, &endptr2, 10);// parse string to long int
		number.Kind = endptr1 == endptr2 ? ReturnKind_Int : ReturnKind_Float;

		Token->Kind = TokenKind_Number;
		Token->Data = number;

		return true;
	}
	if (isalnum(a) || a == '_')
	{
		Token->Kind = TokenKind_Identifier;
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

		}

		if (strcmp(Token->Data.Identifier, "func") == 0) // the first statement should be this 
			Token->Kind = TokenKind_Func;
		else if (strcmp(Token->Data.Identifier, "return") == 0)
			Token->Kind = TokenKind_Return;

		return true;

	}
	if (a == '\"') {  // for the first "

		Tokenizer->Position++;
		for (int iter = 0; Tokenizer->Position < Tokenizer->Length; iter++) {
			char b = Tokenizer->Input[Tokenizer->Position];
			if ((b == '\"')) { // to get to the end of the string
				Tokenizer->Position++;
				Token->Kind = TokenKind_String;
				return true;
			}
			if (iter >= MAX_IDENTIFIER_SIZE) {
				printf("Too long String\n");
				exit(-1);
			}
			Token->Data.Identifier[iter] = b;
			Tokenizer->Position++;
			return true;
		}
	}

	printf("invalid character: %c\n", a);
	exit(1);

	return false;
}
void AdvanceToken(Parser* parser)
{   
	//the main funtion which will forward the parsing of the token
	parser->Parsing = Tokenize(&parser->Lex, &parser->Current);
}
Parser StartParsing(const char* str, int length)
{
	Parser parser = {};
	parser.Lex.Input = str;
	parser.Lex.Position = 0;
	parser.Lex.Length = length;
	AdvanceToken(&parser);
	return parser;
}