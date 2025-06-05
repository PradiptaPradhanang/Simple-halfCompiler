#pragma once
#include "Parser.h"

bool Parsing(Parser* parser)
{
	return parser->Parsing;
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


bool CheckToken(Parser* parser, TokenKind Kind)
{
	Token token = {};
	return AcceptToken(parser, Kind, &token);
}
ExprNode* ExprNodeCreate(ExprKind Kind, Token *token)
{
	ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode)); // allocate size for each node
	if (node != 0)
	{
		memset(node, 0, sizeof(*node));
		node->Kind = Kind;
		node->SRCToken = *token;
	}
	return node;

}

FunctionArguments ParseFunctionArguments(Parser* parser);
ExprNode* ParseExpression(Parser* parser, bool start, int prev_prec);

ExprNode* ParseSubexpression(Parser* parser, bool start)
{
	// mainly for parsing numbers
	Token token = {};
	if (AcceptToken(parser, TokenKind_Number, &token))
	{
		ExprNode* expr = ExprNodeCreate(ExprKind_Number, &token);
		return expr;
	}
	//parse for variable
	if (AcceptToken(parser, TokenKind_Identifier, &token))
	{
		//Token tmpToken = token;
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
				if (desc->ArgCount != -1 && desc->ArgCount != expr->FuncArgs.Count) 
				{
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
				printf("expected close brace \n");
				exit(-1);
			}
			return expr;
		}
		else
		{ // for a normal identifier which isnot related to any kind of function
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

	if (start) // for this first instance mainly for unary operators
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

FunctionArguments ParseFunctionArguments(Parser* parser)
{
	FunctionArguments args;
	memset(&args, 0, sizeof(args));
	///since it is inside a function, closebrace indicates end of the function
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

ExprNode* ParseExpression(Parser* parser,bool start,int prev_prec)
{
	if (start) /// a return token can be present only once in the function body for now
	{
		Token token = {};
			if (AcceptToken(parser, TokenKind_Return, &token))
			{
				ExprNode* expr = ExprNodeCreate(ExprKind_Return, &token);
				expr->LeftNode = ParseExpression(parser, true, -1);
				return expr;
			}
	}
	ExprNode* left = ParseSubexpression(parser, true);

	while (Parsing(parser))
	{  
		if (start)// for validating only one equal sign is valid here
		{
			Token token = {};
			if (AcceptToken(parser, TokenKind_Equal, &token))
			{
				if (left->Kind != ExprKind_Identifier)
				{
					printf("the left side of the assignemnt should be identifieri \n");
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

		for (int iter = 0; iter < ArrayCount(BinaryOperatorInput); ++iter) 
		{
			if (parser->Current.Kind == BinaryOperatorInput[iter])
			{
				precedence = BinaryOperatorPrecedence[iter];
				binaryOp = iter;
				break;
			}
		}
		if (binaryOp == -1 || precedence <= prev_prec)
			break; /// exit when low precedence operator is found, to stop parsing for that point
		ExprNode* expr = ExprNodeCreate(ExprKind_BinaryOperator, &parser->Current);
		expr->LeftNode = left;
		expr->BinaryOperator = BinaryOperatorOutput[binaryOp];

		AdvanceToken(parser);

		//recursion function 
		expr->RightNode = ParseExpression(parser, false, precedence);
		left = expr;

	}
	return  left;
}

ExprNode* ParseRootExpression(Parser* parser)
{
	ExprNode* expr = ParseExpression(parser, true, -1);
	if (!CheckToken(parser, TokenKind_SemiColon))
	{
		printf("error: expected semicolon \n");
		exit(-1);
	}
	return expr;
}

ExprNode* ParseFunction(Parser *parser)
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
				printf("too many parameters\n");
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

		///////////////// the function body parsing begins from here//////////////
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
		expr->FuncBodyFirstExpr = first.Next; //skip the first temporary node

		return expr;


	}

	printf("expected identifier after func\n");
	exit(-1);

}

ExprNode *ParseDefinition(Parser *parser)
{
	if (CheckToken(parser, TokenKind_Func))
	{
		return ParseFunction(parser);
	}
	printf("only functions are allowed in global definition is allowed");
}
