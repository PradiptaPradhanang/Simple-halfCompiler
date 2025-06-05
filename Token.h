#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <ctype.h>

#define ArrayCount(Array) (sizeof(Array)/sizeof((Array)[0]))

#define MAX_IDENTIFIER_SIZE 128
#define MAX_VARIABLE_COUNT 1024
#define MAX_ARGUMENT_COUNT 64

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
	TokenKind_OpenCurlyBrace,// {
	TokenKind_CloseBrace,// )
	TokenKind_CloseCurlyBrace,// }
	TokenKind_BitwiseAND, // &
	TokenKind_BitwiseOR, // |
	TokenKind_EqualEqual,// ==
	TokenKind_BitwiseNot,  // ~
	TokenKind_Return,
	TokenKind_Func,
};

enum ValueType
{
	ReturnKind_Int,
	ReturnKind_Float,
	ReturnKind_Char

};

struct Value
{
	ValueType Kind;
	int Int;
	double Float;
	char Identifier[MAX_IDENTIFIER_SIZE];
};

struct Token
{
	TokenKind Kind;
	int64_t IdentifierLen; // bettter than int because it is perfect for precision but only works for 64 bit ; also int has a different size on 32 bits
	Value Data;
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
const char SingleCharTokenInput[] = {
 ',', '|', '&', '=', ';', '+', '-', '*', '/', '(', ')', '&', '|', '=', '~', '{', '}'
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
	TokenKind_BitwiseNot,  // ~
	TokenKind_OpenCurlyBrace,
	TokenKind_CloseCurlyBrace,
};

struct DoubleCharToken {

	char A, B;
};

const DoubleCharToken DoubleCharTokenInput[] = { {'=', '='}, { '|', '|'}, { '&', '&' } };
const TokenKind   DoubleCharTokenOutput[] ={
	TokenKind_EqualEqual, TokenKind_BitwiseOR, TokenKind_BitwiseAND
};

Parser StartParsing(const char* str, int length);
void AdvanceToken(Parser* parser);
bool Tokenize(Tokenizer* Tokenizer, Token* Token);