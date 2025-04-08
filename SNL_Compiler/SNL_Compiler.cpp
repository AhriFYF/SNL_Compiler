#include "main.h"

// 全局变量
string SMLinputfile = "text\\input.txt";
string Lexicalfile = "text\\Lexical Analysis.txt";

string Syntaxfile = "text\\Syntax Analysis.txt";
string tokenfile = "text\\output.txt";

string Symboltablefile = "text\\Symbol table.txt";
string Semanticfile = "text\\Semantic Analysis.txt";

int size1 = 0;		   // token序列的长度
vector<Token> token;   // token序列
int subscript = 0;	   // 语法分析程序中的下标，标记读到哪个token序列
int size2 = 0;		   // 用来控制树的层次结构
int debugtest = 0;	   // 用来调试
int maxlevel = 0;	   // 最大层次
int currentOffset = 0; // 偏移量

// 从文件读取 SNL 源代码
string readFile(const string &filename)
{
	ifstream file(filename);
	if (!file)
	{
		cerr << "无法打开文件: " << filename << endl;
		exit(1);
	}
	stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

int main()
{
	// 存放输入字符串
	char tmp = ' ';

	string input = readFile(SMLinputfile);
	ofstream Lexical(Lexicalfile);
	ofstream Syntax(Syntaxfile);
	ofstream Symboltable(Symboltablefile);
	ofstream Semantic(Semanticfile);

	if (!Lexical)
	{
		cerr << "无法创建文件: " << Lexicalfile << endl;
		return 1;
	}
	if (!Syntax)
	{
		cerr << "无法创建文件: " << Syntaxfile << endl;
		return 1;
	}
	if (!Semantic)
	{
		cerr << "无法创建文件: " << Semanticfile << endl;
		return 1;
	}

	int len = input.size();
	for (int i = 0; i < len; i++)
	{
		cout << input[i];
	}
	printf("\n");
	system("pause");
	WinExec("text\\a.exe", SW_SHOWNORMAL);
	Sleep(1000);
	printf("\n");
	system("pause");

	std::ifstream file(tokenfile);
	std::string line;
	while (std::getline(file, line))
	{
		// 查找分隔符
		size_t comma_pos = line.find(',');

		// 处理无效行
		if (comma_pos == std::string::npos)
		{
			std::cerr << "警告：跳过格式错误行: " << line << std::endl;
			continue;
		}

		// 提取type和value
		std::string type = line.substr(0, comma_pos);
		std::string value = line.substr(comma_pos + 1);

		// 去除value中的换行符（如果有）
		if (!value.empty() && value.back() == '\n')
		{
			value.pop_back();
		}

		// 存入数组
		token.push_back({type, value});
	}

	std::cout << "成功加载 " << token.size() << " 个Token" << std::endl;

	cout << "\n词法分析结果已写入: " << tokenfile << endl;
	printf("\n");
	system("pause");
	// 语法分析器
	Treenode *root = program(Syntax);
	if (root)
	{
		cout << "\n语法分析结果已写入: " << Syntaxfile << endl;
		printf("\n");
		system("pause");
	}
	else
	{
		cout << "\n语法错误！\n";
	}

	// 语义分析-符号表管理
	Node *syntaxTree = parseSyntaxTree(Syntaxfile); // 解析语法树
	SymbolTable *symTable = new SymbolTable();
	BuildSymbolTable(syntaxTree, *symTable);
	cout << "\n程序经过语义分析器之后的符号表和语义错误信息为:" << endl;
	cout << "Symbol Table:" << endl;
	symTable->PrintSymbolTable(Symboltable);
	cout << "符号表已写入: " << Symboltablefile << endl;
	cout << "\nSemantic Analysis Results:" << endl;

	// 语义分析-语义检查
	SymbolNode *Parsedsymboltable = parseSymbolTable(Symboltablefile);			// 解析符号表
	printSymbolTable(Parsedsymboltable, Symboltable);							// 打印解析后的符号表
	mainsemanticAnalysis(syntaxTree, symTable, Semantic, Parsedsymboltable, 0); // 语义分析
	cout << "语义错误信息已写入: " << Semanticfile << endl;

	system("pause");

	Lexical.close();
	Syntax.close();
	Semantic.close();

	return 0;
}