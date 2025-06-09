#pragma once

#include "Parser.h"

struct Variable
{
	char Name[MAX_IDENTIFIER_SIZE];
	Value Value;
};

struct Memory
{
	bool Returned;
	Variable Vars[MAX_VARIABLE_COUNT];
	int VariableCount;
};

Value Evaluate(ExprNode* expr, Memory* mem);
ExprNode* FindFunction(ExprNode* first, const char* name);
typedef Value(*EvaluateBuiltinFunc)(FunctionArguments*, Memory*);// function pointer
void SetCode(ExprNode f);
Value EvaluateFunction(ExprNode* node, Memory* mem);
