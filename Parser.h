#pragma once
#include "Token.h"

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

struct BuiltinFuncDesc
{
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
	BuiltinFunc_Count,
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
const int BinaryOperatorPrecedence[] =
{
	1,
	1,
	2,
	2,
	0,
	0
};

bool Parsing(Parser* parser);
ExprNode* ParseDefinition(Parser* parser);

