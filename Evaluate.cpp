#pragma once
#include "Evaluate.h"

static ExprNode first;

void SetCode(ExprNode f)
{
	first = f;
}

ExprNode* FindFunction(ExprNode* first, const char* name)
{
	for (ExprNode* node = first->Next; node; node = node->Next)
	{

		if (strcmp(node->SRCToken.Data.Identifier, name) == 0)
		{
			return node;
		}
	}

	return nullptr;
}

void CheckOperandType(Value A, Value B)
{
	if (A.Kind != ReturnKind_Float && A.Kind != ReturnKind_Int ||
		B.Kind != ReturnKind_Float && B.Kind != ReturnKind_Int)
	{
		printf("The entered operands are not suitable for arithmetic operations\n");
		exit(-1);
	}

}
Value AddValue(Value A, Value B) {
	CheckOperandType(A, B);
	Value R = {};
	R.Int = A.Int + B.Int;
	R.Float = A.Float + B.Float;
	R.Kind = A.Kind != B.Kind ? ReturnKind_Float : A.Kind;
	return R;
}

Value SubValue(Value A, Value B) {
	CheckOperandType(A, B);
	Value R = {};
	R.Int = A.Int - B.Int;
	R.Float = A.Float - B.Float;
	R.Kind = A.Kind != B.Kind ? ReturnKind_Float : A.Kind;
	return R;
}

Value MulValue(Value A, Value B) {
	CheckOperandType(A, B);
	Value R = {};
	R.Int = A.Int * B.Int;
	R.Float = A.Float * B.Float;
	R.Kind = A.Kind != B.Kind ? ReturnKind_Float : A.Kind;
	return R;
}

Value DivValue(Value A, Value B) {
	CheckOperandType(A, B);
	Value R = {};
	R.Int = A.Int / B.Int;
	R.Float = A.Float / B.Float;
	R.Kind = A.Kind != B.Kind ? ReturnKind_Float : A.Kind;
	return R;
}

Value EqualValue(Value A, Value B) {
	CheckOperandType(A, B);

	Value R = {};

	R.Kind = ReturnKind_Int;

	if (A.Kind == ReturnKind_Int)
	{
		if (B.Kind == ReturnKind_Int)
		{
			R.Int = (A.Int == B.Int);
		}
		else if (B.Kind == ReturnKind_Float)
		{
			R.Int = (A.Int == B.Float);
		}
	}
	else if (A.Kind == ReturnKind_Float)
	{
		if (B.Kind == ReturnKind_Int)
		{
			R.Int = (A.Float == B.Int);
		}
		else if (B.Kind == ReturnKind_Float)
		{
			R.Int = (A.Float == B.Float);
		}
	}
	return R;
}

Value NotEqualValue(Value A, Value B) {
	Value v = EqualValue(A, B);
	v.Int = v.Int ? 0 : 1;
	return v;
}

Value GreaterValue(Value A, Value B) {
	Value v;
	v.Kind = ReturnKind_Int;
	CheckOperandType(A, B);
	v.Int = (A.Float > B.Float);
	return v;
}

Value LessValue(Value A, Value B) {
	Value v;
	v.Kind = ReturnKind_Int;
	CheckOperandType(A, B);
	v.Int = (A.Float < B.Float);
	return v;
}
Value  BitANDValue(Value A, Value B)
{
	CheckOperandType(A, B);
	Value R = {};
	R.Int = A.Int & B.Int;
	R.Float = (int)A.Float & (int)B.Float;
	R.Kind = A.Kind != B.Kind ? ReturnKind_Float : A.Kind;
	return R;
}
Value  BitORValue(Value L, Value R)
{
	CheckOperandType(L, R);
	Value d = {};
	d.Int = L.Int | R.Int;
	d.Float = (int)L.Float | (int)R.Float;
	d.Kind = L.Kind != R.Kind ? ReturnKind_Float : L.Kind;
	return d;
}
Value Sum(FunctionArguments* args, Memory* mem) {
	Value d = {};
	for (int iter = 0; iter < args->Count; iter++) {
		d = AddValue(d, Evaluate(args->Args[iter], mem));
	}
	return d;
}

Value Mul(FunctionArguments* args, Memory* mem) {
	Value d = {};
	d.Int = 1;
	d.Float = 1;
	d.Kind = ReturnKind_Int;
	for (int iter = 0; iter < args->Count; iter++) {
		d = MulValue(d, Evaluate(args->Args[iter], mem));
	}
	return d;
}
Value Print(FunctionArguments* args, Memory* mem) {
	Value d = {};
	for (int iter = 0; iter < args->Count; iter++) {
		d = Evaluate(args->Args[iter], mem);
		if (d.Kind == ReturnKind_Float)
		{
			printf("%f", d.Float);
		}
		else if (d.Kind == ReturnKind_Int)
		{
			printf("%d", d.Int);
		}
		else
		{
			printf("%s", d.Identifier);
		}
		printf(" ");
	}
	printf("\n");
	return d;
}
Value Min(FunctionArguments* args, Memory* mem) {
	Value d = {};
	d.Float = DBL_MAX;
	d.Int = INT32_MAX;
	d.Kind = ReturnKind_Int;
	for (int iter = 0; iter < args->Count; iter++)
	{
		Value n = Evaluate(args->Args[iter], mem);
		CheckOperandType(n, d);
		if (n.Float < d.Float) d = n;
	}
	return d;
}
Value Max(FunctionArguments* args, Memory* mem)
{
	Value d = {};
	d.Float = DBL_MAX;
	d.Int = INT32_MAX;
	d.Kind = ReturnKind_Int;
	for (int iter = 0; iter < args->Count; iter++)
	{
		Value n = Evaluate(args->Args[iter], mem);
		CheckOperandType(n, d);
		if (n.Float > d.Float) d = n;
	}
	return d;
}
Value Sin(FunctionArguments* args, Memory* mem)
{
	Value d = {};
	d = Evaluate(args->Args[0], mem);
	Value r;
	CheckOperandType(d, d);
	r.Float = sin(d.Float);
	r.Kind = ReturnKind_Float;
	r.Int = (int)r.Float;
	return r;
}
Value Cos(FunctionArguments* args, Memory* mem)
{
	Value d = {};
	d = Evaluate(args->Args[0], mem);
	Value r;
	CheckOperandType(d, d);
	r.Float = cos(d.Float);
	r.Kind = ReturnKind_Float;
	r.Int = (int)r.Float;
	return r;
}
Value Tan(FunctionArguments* args, Memory* mem)
{
	Value d = {};
	d = Evaluate(args->Args[0], mem);
	Value r;
	CheckOperandType(d, d);
	r.Float = tan(d.Float);
	r.Kind = ReturnKind_Float;
	r.Int = (int)r.Float;
	return r;
}
typedef Value(*EvaluateBuiltinFunc)(FunctionArguments*, Memory*);
//FUNCTION pointer////

static EvaluateBuiltinFunc BuildinFuncEvaluateTable[] = {
	Sum,Mul,Min, Max,Sin,Cos,Tan,Print

};
Value EvaluateBuiltInFunction(ExprNode* expr, Memory* mem) {
	if (expr->Func < ArrayCount(BuildinFuncEvaluateTable))
		return BuildinFuncEvaluateTable[expr->Func](&expr->FuncArgs, mem);
	printf("invalid builtin function\n");
	exit(-1);
}
Variable* SearchVariable(Memory* mem, char* varName, bool insert_if_not_found = false)
{
	for (int i = 0;i < mem->VariableCount;i++)
	{
		if (!strcmp(varName, mem->Vars[i].Name))  // if string matches then 0 is returned
		{
			return &mem->Vars[i];
		}
	}

	if (insert_if_not_found)
	{
		if (mem->VariableCount == MAX_VARIABLE_COUNT)
		{
			printf("mem overflow\n");
			exit(-1);
		}

		strcpy_s(mem->Vars[mem->VariableCount].Name, varName);
		Variable* var = &mem->Vars[mem->VariableCount++];

		return var;
	}

	return NULL;
}

Value Evaluate(ExprNode* expr, Memory* mem)
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
			Value v = Evaluate(expr->LeftNode, mem);
			v.Int = -v.Int;
			v.Float = -v.Float;
			return v;
		}
	}
	if (expr->Kind == ExprKind_BinaryOperator)
	{
		Value Left = Evaluate(expr->LeftNode, mem);
		Value Right = Evaluate(expr->RightNode, mem);
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
		else if (expr->BinaryOperator == BinaryOperator_CompareEqual) {
			return EqualValue(Left, Right);
		}
		else if (expr->BinaryOperator == BinaryOperator_CompareNotEqual) {
			return NotEqualValue(Left, Right);
		}
		else if (expr->BinaryOperator == BinaryOperator_CompareGreaterThan) {
			return GreaterValue(Left, Right);
		}
		else if (expr->BinaryOperator == BinaryOperator_CompareLessThan) {
			return LessValue(Left, Right);
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
			strcpy_s(mem->Vars[mem->VariableCount].Name, varname);
			var = &mem->Vars[mem->VariableCount++];
		}
		var->Value = Evaluate(expr->RightNode, mem);



		return var->Value;
	}
	if (expr->Kind == ExprKind_BuiltinFuncCall) {
		return EvaluateBuiltInFunction(expr, mem);
	}
	if (expr->Kind == ExprKind_UserDefinedFuncCall)
	{
		const char* name = expr->SRCToken.Data.Identifier;
		ExprNode* calledFunc;
		if (calledFunc = FindFunction(&first, name))
		{
			if (calledFunc->FuncParams.Count != expr->FuncArgs.Count)
			{
				printf("invalid number of function args\n");
				exit(-1);
			}

			Memory* funcMem = (Memory*)calloc(1, sizeof(Memory));  // Alternative approach  for allocating memory

			//	printf("function %s was found\n", name);
			for (int i = 0;i < expr->FuncArgs.Count;i++)
			{
				Variable* var = SearchVariable(funcMem, calledFunc->FuncParams.Params[i].Data.Identifier, true);
				var->Value = Evaluate(expr->FuncArgs.Args[i], mem);
			}

			////	printf("function %s was found\n", name);
			//for (int i = 0;i < expr->FuncArgs.Count;i++)
			//{
			//	Value val = Evaluate(expr->FuncArgs.Args[i], mem);
			//	calledFunc->FuncArgs.Args[i] = expr->FuncArgs.Args[i];
			//	Variable* varPtr = SearchVariable(mem, expr->FuncArgs.Args[i]->SRCToken.Data.Identifier);
			//	strcpy_s(funcMem->Vars[i].Name, calledFunc->FuncParams.Params[i].Data.Identifier);
			//	funcMem->Vars[i].Value = varPtr->Value;
			//	funcMem->VariableCount++;
			//	calledFunc->FuncArgs.Count++;
			//}

			Value d = EvaluateFunction(calledFunc, funcMem);
			free(funcMem);
			return d;
		}
		else
		{
			printf("no function was found");
		}
	}

	if (expr->Kind == ExprKind_String)
	{
		Value d = {};
		strcpy_s(d.Identifier, expr->SRCToken.Data.Identifier);
		d.Kind = ReturnKind_Char;
		return d;
	}

	if (expr->Kind == ExprKind_Block)
	{
		Value v = {};
		for (ExprNode* node = expr->Child; node; node = node->Next)
		{
			v = Evaluate(node, mem);
			if (mem->Returned)
				return v;
		}
		return v;
	}

	if (expr->Kind == ExprKind_IF)
	{
		Value val = Evaluate(expr->Child, mem);

		if (val.Int)
		{
			return Evaluate(expr->LeftNode, mem);
		}
		else
		{
			if (expr->RightNode)
				Evaluate(expr->RightNode, mem);
		}

		Value value = {};
		return value;
	}

	if (expr->Kind == ExprKind_Return)
	{
		Value v = Evaluate(expr->LeftNode, mem);
		mem->Returned = true;
		return v;
	}

	Value value = {};
	return value;
}

Value EvaluateFunction(ExprNode* node, Memory* mem)
{
	return Evaluate(node->Child, mem);
}
