# CFGParser

C++ source code parser for operator indexing and for building control flow graph (as edge list).
CFGParser implemented both as DLL library and standalone EXE.

DLL input:
- CodeAndPairs* cpstruct – pointer to struct with 2 strings (output code string and edge list string);
- Const char* code_in – input code string.

DLL output:
- CodeAndPairs* cpstruct – pointer to struct with 2 strings (indexed output code string and builded edge list string).

EXE input:
- File with input code (as argument): CFGParser.exe file.cpp (default file "ccode.txt");

EXE output:
- Indexed code and graph pairs outputted to console

Classes:
- Parser – main class responsible for parsing, operator indexing and graph building;
- GrammarCompiler – contains grammar rules for parsing;
- CFGpairs – Parser's helping class for code indexing and graph building.	


## Example:

Input C++ code:
```c#
void main()
{
	int e = rand() % 100;
	
	if (e > 10) e = 10;
	else {
		cout << "Lucky\n";
	}
	
	cin >> e;
	
	switch (e)
	{
		case 1: 
			func1(e);
		case 2: 
			func2(e);
			break;
		default: 
			cout << "Error\n";
	}
	
	cout << "Result: " << e;
	
	while (true) {
		e = myrand();
				
		goto someLBL;
		DEAD_Statement;
		
		someLBL: 
		if (e == 777) break;
	}
}
```

Output:
<pre>
File opened

OUTPUT CODE:
</pre>
```c#
void main()
{
/* 1 */ int e = rand() % 100;
/* 1 */ if (e > 10)
	/* 2 */ e = 10;
/* 3 */ else
	{
	/* 3 */ cout << "Lucky\n";
	}
/* 4 */
/* 4 */ cin >> e;
/* 4 */ switch (e)
	{
	/* 5 */ case 1:
	/* 5 */ func1(e);
	/* 6 */ case 2:
	/* 6 */ func2(e);
	/* 6 */ break;
	/* 7 */ default:
	/* 7 */ cout << "Error\n";
	}
/* 8 */
/* 8 */ cout << "Result: " << e;
/* 8 */ while (true)
	{
	/* 9 */ e = myrand();
	/* 9 */ goto someLBL;
	/* 10 */ DEAD_Statement;
	/* 11 */ someLBL:
	/* 11 */ if (e == 777)
		/* 12 */ break;
	/* 13 */
	}
/* 14 */
}
```
<pre>
CFG PAIRS:

START -> 1;
1 -> 2;
1 -> 3;
2 -> 4;
3 -> 4;
4 -> 5;
5 -> 6;
4 -> 6;
4 -> 7;
7 -> 8;
6 -> 8;
8 -> 9;
10 -> 11;
9 -> 11;
11 -> 12;
11 -> 13;
13 -> 8;
12 -> 14;
8 -> 14;
14 -> EXIT;
</pre>

