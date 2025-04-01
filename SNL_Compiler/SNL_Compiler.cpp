#include "main.h"

//全局变量
string SMLinputfile = "text\\input.txt";
string Lexicalfile = "text\\Lexical Analysis.txt";
string Syntaxfile = "text\\Syntax Analysis.txt";
string Symboltablefile = "text\\Symbol table.txt";
string Semanticfile = "text\\Semantic Analysis.txt";
string Targetcodefile = "text\\Target code generation.txt";

int size1 = 0;			//token序列的长度
vector<Token> token;	//token序列
int subscript = 0;		//语法分析程序中的下标，标记读到哪个token序列
int size2 = 0;			//用来控制树的层次结构
int debugtest = 0;		//用来调试
int maxlevel = 0;		//最大层次
int currentOffset = 0;  //偏移量

//分界符的命名+ | - | *| / | ( | ) | [ | ] | ; | . | < | : | = | ' | := | > | " | ,
char SingleDelimiter[18][20] = { "PLUS","MINUS","TIMES","OVER","LPAREN","RPAREN",
" LMIDPAREN","RMIDPAREN", "SEMI","DOT","LT","COLON","EQ","COMMA","ASSIGN","RT","SY","JSP1" };

// 保留字的命名,为了实现映射关系
string  reservedWords[21] = { "program","type","var","procedure","begin","end","array","of","record","if","then","else",
"fi","while","do","endwh","read","write","return","integer","char" };
string reservedWords1[21] = { "PROGRAM","TYPE","VAR","PROCEDURE","BEGIN","END","ARRAY","OF","RECORD","IF","THEN","ELSE",
"FI","WHILE","DO","ENDWH","READ","WRITE","RETURN","INTEGER","CHAR" };

// 从文件读取 SNL 源代码
string readFile(const string& filename) {
	ifstream file(filename);
	if (!file) {
		cerr << "无法打开文件: " << filename << endl;
		exit(1);
	}
	stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

int main() {
	//存放输入字符串
	char tmp = ' ';

	string input = readFile(SMLinputfile);
	ofstream Lexical(Lexicalfile);
	ofstream Syntax(Syntaxfile);
	ofstream Symboltable(Symboltablefile);
	ofstream Semantic(Semanticfile);
	ofstream Targetcode(Targetcodefile);

	if (!Lexical) {
		cerr << "无法创建文件: " << Lexicalfile << endl;
		return 1;
	}
	if (!Syntax) {
		cerr << "无法创建文件: " << Syntaxfile << endl;
		return 1;
	}
	if (!Semantic) {
		cerr << "无法创建文件: " << Semanticfile << endl;
		return 1;
	}
	if (!Targetcode) {
		cerr << "无法创建文件: " << Targetcodefile << endl;
		return 1;
	}

	int len = input.size();
	for (int i = 0; i < len; i++) {
		cout << input[i];
	}

	//词法分析
	generatetoken(input, len); //将字符串变成token序列
	printtoken(Lexical, token); //将token进行输出
	cout << "\n词法分析结果已写入: " << Lexicalfile << endl;

	//语法分析器
	Treenode* root = program(Syntax);

	//语义分析-符号表管理
	Node* syntaxTree = parseSyntaxTree(Syntaxfile);						//解析语法树
	//PrintSyntaxTree(syntaxTree);										//打印解析后的语法树
	SymbolTable* symTable = new SymbolTable();
	BuildSymbolTable(syntaxTree, *symTable);
	cout << "\n程序经过语义分析器之后的符号表和语义错误信息为:" << endl;
	cout << "Symbol Table:" << endl;
	symTable->PrintSymbolTable(Symboltable);
	cout << "符号表已写入: " << Symboltablefile << endl;
	cout << "\nSemantic Analysis Results:" << endl;
	//语义分析-语义检查
	SymbolNode* Parsedsymboltable = parseSymbolTable(Symboltablefile);					//解析符号表
	printSymbolTable(Parsedsymboltable, Symboltable);									//打印解析后的符号表
	mainsemanticAnalysis(syntaxTree, symTable, Semantic, Parsedsymboltable, 0);			//语义分析
	cout << "语义错误信息已写入: " << Semanticfile << endl;

	//目标代码生成

	Lexical.close();
	Syntax.close();
	Semantic.close();
	Targetcode.close();

	return 0;
}