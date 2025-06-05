#pragma once

#include "Parser.h"

struct Variable
{
	char Name[MAX_IDENTIFIER_SIZE];
	Value Value;
};

struct Memory
{
	Value Ans;
	Variable Vars[MAX_VARIABLE_COUNT];
	int VariableCount;
};

Value Evaluate(ExprNode* expr, Memory* mem);
ExprNode* FindFunction(ExprNode* first, const char* name);
typedef Value(*EvaluateBuiltinFunc)(FunctionArguments*, Memory*);// function pointer
void EvaluateRootExpr(ExprNode* expr, Memory* mem);
void SetCode(ExprNode f);
void EvaluateFunction(ExprNode* node, Memory* mem);
