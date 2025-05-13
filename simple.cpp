
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
bool Parsing(Parser* parser) {
	return parser->Parsing;
}
Parser StartParsing(const char* str, int length) {
	Parser parser = {};
	parser.Lex.Input = str;
	parser.Lex.Position = 0;
	parser.Lex.Length = length;
	AdvanceToken(&parser);
	return parser;
}


char* ReadFile(const char* filepath)
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

///////////////////////////////////////////////exprnode///////////////////////
struct BuiltinFuncDesc {
	const char* Name;
	int ArgCount;
};
static BuiltinFuncDesc BuiltinFuncDescs[] = {
	{ "sum", -1 },
	{ "mul", -1 },
	{ "min", -1 },
	{ "max", -1 },
	{ "sin", 1 },
	{ "cos", 1 },
	{ "tan", 1 },
	{"print", -1},
};


enum BuiltInFunc {
	BuiltinFunc_Sum,
	BuiltinFunc_Mul,
	BuiltinFunc_Min,
	BuiltinFunc_Max,
	BuiltinFunc_Sin,
	BuiltinFunc_Cos,
	BuiltinFunc_Tan,
	BuiltinFunc_Print,
	BuiltinFunc_Count,//8 defualt value 
};
enum ExprKind
{

	ExprKind_Number,
	ExprKind_Identifier,
	ExprKind_BuiltinFunc,
	ExprKind_String,
	ExprKind_BinaryOperator,
	ExprKind_UnaryOperator,
	ExprKind_Assignment,

};

struct ExprNode;
struct FunctionArguments
{
	ExprNode* Args[MAX_ARGUMENT_COUNT];
	int Count;
};

//function declaration
ExprNode* ParseExpression(Parser* parser, bool start, int prec);

FunctionArguments ParseFunctionArguments(Parser* parser)
{
	FunctionArguments args;
	memset(&args, 0, sizeof(args));
	if (parser->Current.Kind == TokenKind_CloseBrace)
		return args;
	while (Parsing(parser))
	{
		if (args.Count == MAX_ARGUMENT_COUNT)
		{
			printf("Too many parameters in the function\n");
			exit(-1);
		}
		args.Args[args.Count] == ParseExpression(parser, true, -1);
		args.Count += 1;
		if (!CheckToken(parser, TokenKind_Comma))
			break;

	}
	return args;
}


enum UnaryOperatorKind
{
	UnaryOperator_Plus,
	UnaryOperator_Minus,
	UnaryOperator_BitwiseNot,
};

enum BinaryOperatorKind 
{
	BinaryOperator_Plus,
	BinaryOperator_Minus,
	BinaryOperator_Multiply,
	BinaryOperator_Divide,
	BinaryOperator_BitwiseAnd,
	BinaryOperator_BitwiseOr,
};

struct ExprNode
{
	ExprKind Kind;
	Token SRCToken;
	ExprNode* LeftNode;
	ExprNode* RightNode;
	BuiltInFunc Func;
	FunctionArguments FuncArgs;
	BinaryOperatorKind BinaryOperator;
	UnaryOperatorKind UnaryOperator;
};

static ExprNode ExprNodeBuffer[8192];
static int ExprNodePos = 0;

ExprNode* ExprNodeCreate(ExprKind Kind, Token* token)
{
	if (ExprNodePos == ArrayCount(ExprNodeBuffer))
	{
		printf("Error: Not enough memory.");
		exit(1);
	}
	ExprNode* node = &ExprNodeBuffer[ExprNodePos];
	ExprNodePos += 1;
	memset(node, 0, sizeof(*node));
	node->Kind = Kind;
	node->SRCToken = *token;
	return node;

}

bool AcceptToken(Parser* parser, TokenKind kind, Token* token)
{
	if (parser->Current.Kind == kind)
	{
		*token = parser->Current;// this will hold the current token token as the parser->Current will be updated on AdvanceTokenCall
		AdvanceToken(parser);
		return true;
	}
	return false;
}
bool CheckToken(Parser* parser, TokenKind kind)
{
	Token token = { };
	return AcceptToken(parser, kind, &token);
}


ExprNode* ParseSubexpression(Parser* parser, bool start)
{

	// mainly done to parse numbers
	Token token = {};
	if (AcceptToken(parser, TokenKind_Number, &token))
	{
		ExprNode* expr = ExprNodeCreate(ExprKind_Number, &token);
		return expr;
	}

	if (AcceptToken(parser, TokenKind_Identifier, &token))
	{
		// Token tmpToken = token;
		////////////for built in function
		if (CheckToken(parser, TokenKind_OpenBrace))
		{
			BuiltinFuncDesc* desc = NULL;
			BuiltInFunc func = BuiltinFunc_Sum; // let us assume first that it is sum
			for (int i = 0;i < BuiltinFunc_Count;i++)
			{
				if (!strcmp(BuiltinFuncDescs[i].Name, token.Data.Identifier)) // the return for strcmp is 0;
				{
					desc = &BuiltinFuncDescs[i];
					func = (BuiltInFunc)i;
					break;
				}
			}

			if (!desc)
			{
				printf("%s function is undefined\n", token.Data.Identifier);
				exit(-1);
			}
			ExprNode* expr = ExprNodeCreate(ExprKind_BuiltinFunc, &token);
			expr->FuncArgs = ParseFunctionArguments(parser);// for function 
			expr->Func = func;

			if (desc->ArgCount != -1 && desc->ArgCount != expr->FuncArgs.Count) {
				printf("expected %d number of arguments but got %d arguments for function %s\n",
					desc->ArgCount, expr->FuncArgs.Count, desc->Name);
				exit(-1);
			}

			if (!CheckToken(parser, TokenKind_CloseBrace)) 
			{
				printf("expected close brace\n");
				exit(-1);
			}
			return expr;
		}
		else {
			ExprNode* expr = ExprNodeCreate(ExprKind_Identifier, &token);
			return expr;
		}




	}

	if (CheckToken(parser, TokenKind_OpenBrace)) {
		ExprNode* expr = ParseExpression(parser, true, -1);
		if (!CheckToken(parser, TokenKind_CloseBrace)) {
			printf("error: expected close brace\n");
			exit(1);
		}
		return expr;
	}
}

ExprNode *ParseExpression(Parser* parser, bool start, int prev_prec)
{
	ExprNode* left = ParseSubexpression(parser, true);
	ExprNode* expr;
	return  left ;
}

ExprNode* ParseRootExpression(Parser* parser) {
	ExprNode* expr = ParseExpression(parser, true, -1);
	if (!CheckToken(parser, TokenKind_SemiColon)) {
		printf("error: expected semicolon\n");
		exit(1);
	}
	return expr;
}
///////////////////////////////////////////////////

void  main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("error: expected file path\n");
		exit(2);
	}

	const char* filepath = argv[1];
	char* input = ReadFile(filepath);
	Parser parser = StartParsing(input, strlen(input));

	Memory memory;
	memset(&memory, 0, sizeof(memory));
	while (Parsing(&parser))
	{

		ExprNode* expr = ParseRootExpression(&parser);
		/*PrintExpr(expr, 0);
		EvaluateRootExpr(expr, &memory);
		ExprNodeReset();
		if (memory.Ans.Kind == ReturnKind_Float) {
			printf("Float = %f\n", memory.Ans.Float);
		}
		else if (memory.Ans.Kind == ReturnKind_Int) {
			printf("Int = %ld\n", memory.Ans.Int);
		}
		else if (memory.Ans.Kind == ReturnKind_Char) {
			printf("String = %s\n", memory.Ans.Identifier);
	   // }*/
	   // printf("=================================================================================\n");*/
	   ///*
	}




}
