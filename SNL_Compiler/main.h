#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <vector>
#include <windows.h>
#include <stack>
#include <memory>
#include <list>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <filesystem>

using namespace std;

#define distant 4 // 控制*的长度

extern string SMLinputfile;
extern string Lexicalfile;
extern string Syntaxfile;
extern string Symboltablefile;
extern string Semanticfile;
extern string Intermediatecodefile;
extern string Targetcodefile;
extern string lexiprogramfile;
extern string tokenfile;

// 定义token序列的结构
struct Token
{
	string value1;
	string value2;
};

// 节点的具体类型
struct Specificnode
{
	string dec = "";
	string stmt = "";
	string exp = "";
};

// 符号表项结构
struct SymbolEntry
{
	string name;	 // 标识符名
	string type;	 // 类型（INTEGER, BOOL等）
	string procName; // 过程名
	int level;		 // 作用域层级
	int offset;		 // 在活动记录中的偏移量
	int upperbound;
	int lowerbound;
	SymbolEntry *next; // 链表指针

	SymbolEntry(const string &n, const string &t, int lv, int off, const string &pn, int low, int up)
		: name(n), type(t), level(lv), offset(off), next(nullptr), procName(pn), lowerbound(low), upperbound(up) {}
};

// 定义语法分析树的节点
struct Treenode
{
	Treenode *child[10];
	Treenode *sibling;
	int idchild = 0;		   // 儿子的个数
	string nodekind;		   // 节点类型
	Specificnode specificnode; // 详细类型
	int idnum = 0;			   // 标识符个数
	string value;			   // 节点的值（有的节点只有一个标识符。也用value了）
	string id[10];			   // 部分标识符的值
};

// 解析后的语法树节点结构
struct Node
{
	string type;
	string name;
	string varType;
	string leftVar, rightVar;
	string isparam;
	string lowerBound = 0; // 下界（数组）
	string upperBound = 0; // 上界（数组）
	int size = 0;		   // 数组大小
	vector<Node *> children;
	Node *parent = nullptr; // 父节点指针

	// Node(string t, string n = "", string vType = "", string par = "") : type(t), name(n), varType(vType), isparam(par) {}
	Node(string t, string n = "", string vType = "", string par = "", string lb = 0, string ub = 0)
		: type(t), name(n), varType(vType), isparam(par), lowerBound(lb), upperBound(ub)
	{
	}
};

// 解析后的符号表节点结构
struct SymbolNode
{
	string type;				  // 类型、变量、过程、参数
	string name;				  // 名称
	int level;					  // 作用域层级
	SymbolNode *child = nullptr;  // 子节点
	SymbolNode *parent = nullptr; // 父节点

	SymbolNode(string t, string n = "", int i = 0) : type(t), name(n), level(i) {}
};

// 符号表项类型
enum class SymKind
{
	TYPE,
	VAR,
	PROC,
	PARAM
};

// 域表项结构（用于结构体/记录的成员）
struct FieldEntry
{
	string name;	  // 域名
	string type;	  // 类型
	FieldEntry *next; // 链表指针
	SymKind kind;

	FieldEntry(const string &n, const string &t)
		: name(n), type(t), next(nullptr) {}
};

// 域表头结构
struct FieldChain
{
	FieldEntry *head; // 链表头指针
	FieldChain() : head(nullptr) {}
};

struct Quadruple
{
	string op, arg1, arg2, result;
};

// 符号表类
class SymbolTable
{
private:
	int currentLevel = 0;							 // 当前作用域层级
	SymbolEntry *scopeStack[100] = {nullptr};		 // 作用域栈（固定100层）
	SymbolEntry *currentTable = nullptr;			 // 当前符号表链表头
	unordered_map<string, FieldChain *> fieldTables; // 域名 -> 域表映射

	// 类型枚举转字符串
	string KindToString(SymKind kind)
	{
		switch (kind)
		{
		case SymKind::TYPE:
			return "TYPE";
		case SymKind::VAR:
			return "VAR";
		case SymKind::PROC:
			return "PROC";
		case SymKind::PARAM:
			return "PARAM";
		default:
			return "UNKNOWN";
		}
	}

	// 在当前层级查找符号
	bool SearchOneTable(const string &id, SymbolEntry **entry);

public:
	unordered_map<string, string> table;
	SymbolTable *parent;

	// 构造/析构
	SymbolTable(SymbolTable *p = nullptr);
	~SymbolTable();

	// 作用域管理
	void EnterScope();
	void ExitScope();
	void CreateTable();
	void DestroyTable();

	// 符号操作
	bool AddSymbol(const string &type, SymKind kind, const string &name);
	bool AddSymbolHead(const string &type, SymKind kind, const string &name);
	bool AddSymbolType(const string &type, SymKind kind, const string &name);
	bool AddSymbolParam(const string &procName, const string &type, SymKind kind, const string &name);
	bool AddSymbolProc(const string &type, SymKind kind, const string &name);
	bool AddSymbolArray(const string &type, SymKind kind, const string &name, int up, int low);

	// 符号查找
	SymbolEntry *LookupCurrentScope(const string &name);
	SymbolEntry *Lookup(const string &name);
	bool FindEntry(const string &id, const string &flag, SymbolEntry **entry);
	string typereturn(const string &name);

	// 域表操作
	void AddFieldTable(const string &typeName, FieldChain *fields);
	FieldChain *GetFieldTable(const string &typeName);
	bool FindField(const string &id, FieldChain *head, FieldEntry **entry);
	bool FindFieldInTable(const string &typeName, const string &fieldName, FieldEntry **entry);
	bool Enter(const string &id, const string &type, SymbolEntry **entry);

	// 辅助功能
	bool insert(const string &name, const string &type, ofstream &outputFile);
	string lookup(const string &name);
	void PrintSymbolTable(ofstream &outputFile);
	void setcurrentlevel(int i);
	void addcurrentlevel(int i);
	void subcurrentlevel(int i);
};

// 全局变量
extern int size1;
extern vector<Token> token; // token序列
extern int subscript;
extern int size2;
extern int debugtest;
extern int maxlevel;
extern int currentOffset;
extern int num;							   // 记录参数个数
extern int procnum;						   // 记录过程个数
extern vector<vector<string>> paramvector; // 参数节点栈
extern bool isstmtk;					   // 是否是语句节点
extern bool isassignk;					   // 是否是赋值节点
extern int enterdepth;					   // 当前层级深度
extern int isparam;						   // 是否是参数节点
extern bool isconditonk;				   // 是否是条件节点

// 分界符的命名+ | - | *| / | ( | ) | [ | ] | ; | . | < | : | = | ' | := | > | " | ,
extern char SingleDelimiter[18][20];

// 保留字的命名,为了实现映射关系
extern string reservedWords[21];
extern string reservedWords1[21];

// 定义词法分析器的函数
void printwrong();
void printq(int size3, ofstream &outputFile);
void printtoken(ofstream &outputFile, vector<Token> token);
void generatetoken(string input, int len);
Token gettoken(int m);

// 定义语法分析器的函数
Treenode *program(ofstream &outputFile);
Treenode *programhead(ofstream &outputFile);  // 程序头
Treenode *declarehead(ofstream &outputFile);  // 声明
Treenode *typedec(ofstream &outputFile);	  // 类型声明
Treenode *vardec(ofstream &outputFile);		  // 变量声明
Treenode *procdec(ofstream &outputFile);	  // 过程声明
Treenode *typedeclist(ofstream &outputFile);  // 类型声明中的部分函数
Treenode *vardeclist(ofstream &outputFile);	  // 变量声明中的部分函数
Treenode *paramdeclist(ofstream &outputFile); // 过程声明中的部分函数
Treenode *paramlist(ofstream &outputFile);	  // 过程声明中的形参函数
Treenode *probody(ofstream &outputFile);	  // 过程声明中的函数体,后面可以直接用作程序体
Treenode *stmt(ofstream &outputFile);		  // 生成一个语句节点
Treenode *assign1(ofstream &outputFile);	  // 生成赋值表达式
Treenode *write1(ofstream &outputFile);		  // 生成读写表达式
Treenode *read1(ofstream &outputFile);		  // 生成读写表达式
Treenode *if1(ofstream &outputFile);		  // 生成选择表达式

// 定义语义分析器的函数
bool isNumberSimple(const string &s);
Node *parseSyntaxTree(const string &filePath);
void printSyntaxTree(Node *node, int depth = 0);
void PrintSyntaxTree(Node *node, int depth = 0, bool isLastChild = true);
void BuildSymbolTable(Node *node, SymbolTable &symTable);
SymbolNode *parseSymbolTable(const string &filePath);
void printSymbolTable(SymbolNode *node, ofstream &outputFile);
bool isStrictBool(Node *node, SymbolTable *symTable);
void semanticAnalysis(Node *tree, SymbolTable *symTable, ofstream &outputFile, SymbolNode *Parsedsymboltable, int depth, int childnum);
void mainsemanticAnalysis(Node *tree, SymbolTable *symTable, ofstream &outputFile, SymbolNode *Parsedsymboltable, int depth);