
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
	TokenKind_OpenCurlyBrace,// (
	TokenKind_CloseBrace,// )
	TokenKind_CloseCurlyBrace,// )
	TokenKind_BitwiseAND, // &
	TokenKind_BitwiseOR, // |
	TokenKind_EqualEqual,// ==
	TokenKind_BitwiseNot,  // ~
	TokenKind_Return,
	TokenKind_Func,
};
const char* TokenNames[] = {
	"invalid", "Number", "Identifier", ",", "||", "&&", "String", "==", ";", "+", "-", "*", "/", "(", "{", ")", "}", "&", "|", "==", "~", "return", "func"
};


enum ValueType
{
	ReturnKind_Int,
	ReturnKind_Float,
	ReturnKind_Char

};

struct ReturnValue
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
 ',', '||', '&&', '==', ';', '+', '-', '*', '/', '(', ')', '&', '|', '==', '~', '{', '}'
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
	}

	for (int i = 0; i < ArrayCount(SingleCharTokenInput); ++i)
	{
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

		if (strcmp(Token->Data.Identifier, "func") == 0)
			Token->Kind = TokenKind_Func;
		else if (strcmp(Token->Data.Identifier, "return") == 0)
			Token->Kind = TokenKind_Return;

		return true;

	}

	printf("invalid character: %c\n", a);
	exit(1);

	return false;
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
	{"print", -1},
};


enum BuiltInFunc {
	BuiltinFunc_Sum,
	BuiltinFunc_Mul,
	BuiltinFunc_Min,
	BuiltinFunc_Max,
	BuiltinFunc_Print,
	BuiltinFunc_Count,//8 defualt value 
};
enum ExprKind
{
	ExprKind_Number,
	ExprKind_Identifier,
	ExprKind_BuiltinFuncCall,
	ExprKind_UserDefinedFuncCall,
	ExprKind_String,
	ExprKind_BinaryOperator,
	ExprKind_UnaryOperator,
	ExprKind_Assignment,
	ExprKind_Function,
	ExprKind_Return,
};

struct ExprNode;
struct FunctionArguments
{
	ExprNode* Args[MAX_ARGUMENT_COUNT];
	int Count;
};

//function declaration
ExprNode* ParseExpression(Parser* parser, bool start, int prec);
bool CheckToken(Parser* parser, TokenKind kind);
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
		args.Args[args.Count] = ParseExpression(parser, true, -1);
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
	BinaryOperator_BitwiseAND,
	BinaryOperator_BitwiseOR,
};

struct FunctionParameters
{
	Token Params[MAX_ARGUMENT_COUNT];
	int Count;
};

struct ExprNode
{
	ExprKind Kind;
	Token SRCToken;
	ExprNode* Next;
	ExprNode* LeftNode;
	ExprNode* RightNode;
	ExprNode* FuncBodyFirstExpr;
	BuiltInFunc Func;
	FunctionArguments FuncArgs;
	FunctionParameters FuncParams;
	BinaryOperatorKind BinaryOperator;
	UnaryOperatorKind UnaryOperator;
};

const TokenKind UnaryOperatorStartInput[] = {
	TokenKind_Plus,
	TokenKind_Minus,
};
const UnaryOperatorKind UnaryOperatorStartOutput[] = {
	UnaryOperator_Plus,
	UnaryOperator_Minus,
};

const TokenKind BinaryOperatorInput[] = {
	TokenKind_Plus,
	TokenKind_Minus,
	TokenKind_Multiply,
	TokenKind_Divide,
	TokenKind_AND,
	TokenKind_BitwiseOR,
};

const BinaryOperatorKind BinaryOperatorOutput[] = {
	BinaryOperator_Plus,
	BinaryOperator_Minus,
	BinaryOperator_Multiply,
	BinaryOperator_Divide,
	BinaryOperator_BitwiseAND,
	BinaryOperator_BitwiseOR,
};
const int BinaryOperatorPrecedence[] = {
	1,
	1,
	2,
	2,
	0,
	0
};

ExprNode* ExprNodeCreate(ExprKind Kind, Token* token)
{
	ExprNode* node = (ExprNode *)malloc(sizeof(ExprNode));
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

	//parse for variable
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

			ExprNode* expr = nullptr;

			if (desc)
			{
				expr = ExprNodeCreate(ExprKind_BuiltinFuncCall, &token);
				expr->FuncArgs = ParseFunctionArguments(parser);// for function 
				expr->Func = func;

				if (desc->ArgCount != -1 && desc->ArgCount != expr->FuncArgs.Count) {
					printf("expected %d number of arguments but got %d arguments for function %s\n",
						   desc->ArgCount, expr->FuncArgs.Count, desc->Name);
					exit(-1);
				}
			}
			else
			{
				expr = ExprNodeCreate(ExprKind_UserDefinedFuncCall, &token);
				expr->FuncArgs = ParseFunctionArguments(parser);// for function 
				expr->Func = func;
			}

			if (!CheckToken(parser, TokenKind_CloseBrace))
			{
				printf("expected close brace\n");
				exit(-1);
			}

			return expr;
		}
		else
		{
			ExprNode* expr = ExprNodeCreate(ExprKind_Identifier, &token);
			return expr;
		}


	}

	if (CheckToken(parser, TokenKind_OpenBrace))
	{
		ExprNode* expr = ParseExpression(parser, true, -1);
		if (!CheckToken(parser, TokenKind_CloseBrace)) {
			printf("error: expected close brace\n");
			exit(1);
		}
		return expr;
	}

	if (start) // for the first instance
	{
		for (int i = 0;i < ArrayCount(UnaryOperatorStartInput);i++)
		{
			if (AcceptToken(parser, UnaryOperatorStartInput[i], &token))
			{
				ExprNode* expr = ExprNodeCreate(ExprKind_UnaryOperator, &token);
				expr->LeftNode = ParseSubexpression(parser, false);
				expr->UnaryOperator = UnaryOperatorStartOutput[i];
				return expr;
			}
		}
	}

	/*for (int i = 0; i < ArrayCount(UnaryOperatorInput); i++) 
	{
		if (AcceptToken(parser, UnaryOperatorInput[i], &token)) 
		{
			ExprNode* expr = ExprNodeCreate(ExprKind_UnaryOperator, &token);
			expr->LeftNode = ParseSubexpression(parser, false);
			expr->UnaryOperator = UnaryOperatorStartOutput[i];
			return expr;
		}
	}*/
	if (AcceptToken(parser, TokenKind_String, &token))
	{  //parser will contain the current token that has been parsed and the toekn will contain the previous token
		ExprNode* expr = ExprNodeCreate(ExprKind_String, &token);
		return expr;
	}

	printf("error: expected operand\n");
	exit(1);
}



ExprNode *ParseExpression(Parser *parser, bool start, int prev_prec)
{
	if (start)
	{
		Token token = {};
		if (AcceptToken(parser, TokenKind_Return, &token))
		{
			ExprNode *expr = ExprNodeCreate(ExprKind_Return, &token);
			expr->LeftNode = ParseExpression(parser, true, -1);
			return expr;
		}
	}

	ExprNode* left = ParseSubexpression(parser, true);

	while (Parsing(parser))
	{
		if (start) // if the start is not changed to false
		{
			Token token = {};
			if (AcceptToken(parser, TokenKind_Equal, &token))
			{
				if (left->Kind != ExprKind_Identifier)
				{
					printf("The left side of the assignment should be identifier\n");
					exit(-1); 
				}
				ExprNode* expr = ExprNodeCreate(ExprKind_Assignment, &token);
				expr->LeftNode = left;
				expr->RightNode = ParseExpression(parser, false, -1);
				return expr;
			}

		}
		start = false; // start flag is now off
		int precedence = 0;
		int binaryOp = -1;
		for (int iter = 0; iter < ArrayCount(BinaryOperatorInput); ++iter) {
			if (parser->Current.Kind == BinaryOperatorInput[iter])
			{
				precedence = BinaryOperatorPrecedence[iter];
				binaryOp = iter;
				break;
			}
		}
		if (binaryOp == -1 || precedence <= prev_prec)
			break;
		ExprNode* expr = ExprNodeCreate(ExprKind_BinaryOperator, &parser->Current);
		expr->LeftNode = left;
		expr->BinaryOperator = BinaryOperatorOutput[binaryOp];

		AdvanceToken(parser);

		//recursion function 
		expr->RightNode = ParseExpression(parser, false, precedence);
		left = expr;

	}

	return  left ;
}


ExprNode* ParseRootExpression(Parser* parser);

ExprNode* ParseFunction(Parser* parser)
{
	Token token;
	if (AcceptToken(parser, TokenKind_Identifier, &token))
	{
		ExprNode* expr = ExprNodeCreate(ExprKind_Function, &token);

		if (!CheckToken(parser, TokenKind_OpenBrace))
		{
			printf("expected open brace");
			exit(-1);
		}

		bool first_param = true;

		while (1)
		{
			if (first_param)
			{
				if (!AcceptToken(parser, TokenKind_Identifier, &token))
				{
					break;
				}
				first_param = false;
			}
			else
			{
				if (!CheckToken(parser, TokenKind_Comma))
				{
					break;
				}

				if (!AcceptToken(parser, TokenKind_Identifier, &token))
				{
					printf("expected identifier\n");
					exit(-1);
				}
			}

			if (expr->FuncParams.Count == MAX_ARGUMENT_COUNT)
			{
				printf("too many paramnters\n");
				exit(-1);
			}

			expr->FuncParams.Params[expr->FuncParams.Count] = token;
			expr->FuncParams.Count += 1;
		}

		if (!CheckToken(parser, TokenKind_CloseBrace))
		{
			printf("expected close brace");
			exit(-1);
		}

		if (!CheckToken(parser, TokenKind_OpenCurlyBrace))
		{
			printf("expected open curly brace");
			exit(-1);
		}

		ExprNode first = {};
		ExprNode* last = &first;

		while (1)
		{
			ExprNode* node = ParseRootExpression(parser);

			last->Next = node;
			last = node;

			if (CheckToken(parser, TokenKind_CloseCurlyBrace))
			{
				break;
			}
		}

		expr->FuncBodyFirstExpr = first.Next;

		return expr;
	}

	printf("expected identifier after func\n");
	exit(-1);
}

ExprNode* ParseDefination(Parser *parser)
{
	if (CheckToken(parser, TokenKind_Func))
	{
		return ParseFunction(parser);
	}

	printf("only functions are allowed in global definations\n");
	exit(-1);
}

ExprNode* ParseRootExpression(Parser* parser) 
{
	ExprNode* expr = ParseExpression(parser, true, -1);
	if (!CheckToken(parser, TokenKind_SemiColon)) 
	{
		printf("error: expected semicolon\n");
		exit(1);
	}
	return expr;
}
///////////////////////////////////////////////////

ExprNode *FindFunction(ExprNode *first, const char *name)
{
	for (ExprNode *node = first->Next; node; node = node->Next)
	{

		if (strcmp(node->SRCToken.Data.Identifier, name) == 0)
		{
			return node;
		}
	}

	return nullptr;
}
ReturnValue Evaluate(ExprNode* expr, Memory* mem);

ReturnValue AddValue(ReturnValue A, ReturnValue B) {
	ReturnValue R = {};
	R.Int = A.Int + B.Int;
	R.Float = A.Float + B.Float;
	R.Kind = A.Kind != B.Kind ? ReturnKind_Float : A.Kind;
	return R;
}

ReturnValue SubValue(ReturnValue A, ReturnValue B) {
	ReturnValue R = {};
	R.Int = A.Int - B.Int;
	R.Float = A.Float - B.Float;
	R.Kind = A.Kind != B.Kind ? ReturnKind_Float : A.Kind;
	return R;
}

ReturnValue MulValue(ReturnValue A, ReturnValue B) {
	ReturnValue R = {};
	R.Int = A.Int * B.Int;
	R.Float = A.Float * B.Float;
	R.Kind = A.Kind != B.Kind ? ReturnKind_Float : A.Kind;
	return R;
}

ReturnValue DivValue(ReturnValue A, ReturnValue B) {
	ReturnValue R = {};
	R.Int = A.Int / B.Int;
	R.Float = A.Float / B.Float;
	R.Kind = A.Kind != B.Kind ? ReturnKind_Float : A.Kind;
	return R;
}
ReturnValue  BitANDValue(ReturnValue A, ReturnValue B)
{
	ReturnValue R = {};
	R.Int = A.Int & B.Int;
	R.Float = (int)A.Float & (int)B.Float;
	R.Kind = A.Kind != B.Kind ? ReturnKind_Float : A.Kind;
	return R;
}
ReturnValue  BitORValue(ReturnValue L, ReturnValue R)
{
	ReturnValue d = {};
	d.Int = L.Int | R.Int;
	d.Float = (int)L.Float | (int)R.Float;
	d.Kind = L.Kind != R.Kind ? ReturnKind_Float : L.Kind;
	return d;
}
ReturnValue Sum(FunctionArguments* args, Memory* mem) {
	ReturnValue d = {};
	for (int iter = 0; iter < args->Count; iter++) {
		d = AddValue(d, Evaluate(args->Args[iter], mem));
	}
	return d;
}

ReturnValue Mul(FunctionArguments* args, Memory* mem) {
	ReturnValue d = {};
	d.Int = 1;
	d.Float = 1;
	d.Kind = ReturnKind_Int;
	for (int iter = 0; iter < args->Count; iter++) {
		d = MulValue(d, Evaluate(args->Args[iter], mem));
	}
	return d;
}
ReturnValue Print(FunctionArguments* args, Memory* mem) {
	ReturnValue d = {};
	for (int iter = 0; iter < args->Count; iter++) {
		d = Evaluate(args->Args[0], mem);
		if (d.Kind == ReturnKind_Float)
		{
			printf("%f\n", d.Float);
		}
		else if (d.Kind == ReturnKind_Int)
		{
			printf("%d\n", d.Int);
		}
		else
		{
			printf("%s\n", d.Identifier);
		}
	}
	return d;
}
ReturnValue Min(FunctionArguments* args, Memory* mem) {
	ReturnValue d = {};
	d.Float = DBL_MAX;
	d.Int = INT64_MAX;
	d.Kind = ReturnKind_Int;
	for (int iter = 0; iter < args->Count; iter++) 
	{
		ReturnValue n = Evaluate(args->Args[iter], mem);
		if (n.Float < d.Float) d = n;
	}
	return d;
}
ReturnValue Max(FunctionArguments* args, Memory* mem) {
	ReturnValue d = {};
	d.Float = DBL_MAX;
	d.Int = INT64_MAX;
	d.Kind = ReturnKind_Int;
	for (int iter = 0; iter < args->Count; iter++)
	{
		ReturnValue n = Evaluate(args->Args[iter], mem);
		if (n.Float > d.Float) d = n;
	}
	return d;
}

typedef ReturnValue(*EvaluateBuiltinFunc)(FunctionArguments*, Memory*);
//FUNCTION pointer////

static EvaluateBuiltinFunc BuildinFuncEvaluateTable[] = {
	Sum,Mul,Min, Max,Print
	
};
ReturnValue EvaluateBuiltInFunction(ExprNode* expr, Memory* mem) {
	if (expr->Func < ArrayCount(BuildinFuncEvaluateTable))
		return BuildinFuncEvaluateTable[expr->Func](&expr->FuncArgs, mem);
}
Variable* SearchVariable(Memory* mem, char* varName)
{
	for (int i = 0;i < mem->VariableCount;i++)
	{
		if (!strcmp(varName, mem->Vars[i].Name))
		{
			return &mem->Vars[i];
		}
	}
	return NULL;
}
ReturnValue Evaluate(ExprNode* expr, Memory* mem)
{
	if (expr->Kind == ExprKind_Number)
	{
		return expr->SRCToken.Data;
	}
	if (expr->Kind == ExprKind_Identifier)
	{
		Variable* var = SearchVariable(mem, expr->SRCToken.Data.Identifier);
		if (!var)
		{
			if (mem->VariableCount == MAX_VARIABLE_COUNT)
			{
				printf("Variable %s isnot defined \n", expr->SRCToken.Data.Identifier);
				exit(-1);
			}
			
		}
		return var->Value;
	}
	if (expr->Kind == ExprKind_UnaryOperator)
	{
		if (expr->UnaryOperator == UnaryOperator_Plus)
		{
			return Evaluate(expr->LeftNode, mem);
		}
		else if (expr->UnaryOperator == UnaryOperator_Minus)
		{
			ReturnValue v = Evaluate(expr->LeftNode, mem);
			v.Int = -v.Int;
			v.Float = -v.Float;
			return v;
		}
	}
	if (expr->Kind == ExprKind_BinaryOperator)
	{
		ReturnValue Left = Evaluate(expr->LeftNode, mem);
		ReturnValue Right = Evaluate(expr->RightNode, mem);
		if (expr->BinaryOperator == BinaryOperator_Plus)
		{
			return AddValue(Left, Right);
		}
		else if (expr->BinaryOperator == BinaryOperator_Minus) {
			return SubValue(Left, Right);
		}
		else if (expr->BinaryOperator == BinaryOperator_Multiply) {
			return MulValue(Left, Right);
		}
		else if (expr->BinaryOperator == BinaryOperator_Divide) {
			return DivValue(Left, Right);
		}
	}
	if (expr->Kind == ExprKind_Assignment)
	{
		char* varname = expr->LeftNode->SRCToken.Data.Identifier;
		Variable* var = SearchVariable(mem, varname);
		if (!var) 
		{
			if (mem->VariableCount == MAX_VARIABLE_COUNT) {
				printf("Out of Memory. Cannot create new variable\n");
				exit(-1);
			}
			strcpy_s(mem->Vars[mem->VariableCount].Name,varname);
			var = &mem->Vars[mem->VariableCount++];
		}
		var->Value = Evaluate(expr->RightNode, mem);
		return var->Value;
	}
	if (expr->Kind == ExprKind_BuiltinFuncCall) {
		return EvaluateBuiltInFunction(expr, mem);
	}

	if (expr->Kind == ExprKind_String) {
		ReturnValue d = {};
		strcpy_s(d.Identifier, expr->SRCToken.Data.Identifier);
		d.Kind = ReturnKind_Char;
		return d;
	}
}
void EvaluateRootExpr(ExprNode *expr, Memory *mem) 
{
    mem->Ans =Evaluate(expr, mem);
}
 

int  main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("error: expected file path\n");
		exit(2); 
	}

	const char* filepath = argv[1];
	char* input = ReadFile(filepath);
	Parser parser = StartParsing(input, strlen(input));

	ExprNode first = {};
	ExprNode *last = &first;

	Memory memory;
	memset(&memory, 0, sizeof(memory));
	while (Parsing(&parser))
	{
		ExprNode* expr = ParseDefination(&parser);

		const char *name = expr->SRCToken.Data.Identifier;

		if (FindFunction(&first, name))
		{
			printf("redefination of function %s\n", name);
			exit(-1);
		}

		last->Next = expr;
		last = expr;
	}

	ExprNode *main_function = FindFunction(&first, "main");

	if (!main_function)
	{
		printf("main function not defined\n");
		exit(-1);
	}

	if (main_function->FuncParams.Count != 0)
	{
		printf("main function is not allowed to have function parameters\n");
		exit(-1);
	}

	Memory mem;
	memset(&mem, 0, sizeof(mem));
	for (ExprNode *expr = main_function->FuncBodyFirstExpr; expr; expr = expr->Next)
	{
		EvaluateRootExpr(expr, &mem);
		
	}

}
