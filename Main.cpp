#include "Main.h"


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


int  main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("error: expected file path\n");
		exit(2);
	}

	const char* filepath = argv[1];
	char* input = ReadFile(filepath);
	Parser parser = StartParsing(input, strlen(input));// to initialize the token

	ExprNode first = {};

	ExprNode *last = &first;

	Memory memory;
	memset(&memory, 0, sizeof(memory));
	while (Parsing (&parser))
	{
		ExprNode* expr = ParseDefinition(&parser);

		const char* name = expr->SRCToken.Data.Identifier;

		if (FindFunction(&first, name))
		{
			printf("redefination of function %s\n", name);
			exit(-1);
		}

		last->Next = expr;
		last = expr;
	}

	ExprNode* main_function = FindFunction(&first, "main");

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

	SetCode(first);
	EvaluateFunction(main_function, &mem);

}

