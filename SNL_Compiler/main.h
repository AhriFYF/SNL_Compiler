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

#define distant 4 // ����*�ĳ���

extern string SMLinputfile;
extern string Lexicalfile;
extern string Syntaxfile;
extern string Symboltablefile;
extern string Semanticfile;
extern string Intermediatecodefile;
extern string Targetcodefile;
extern string lexiprogramfile;
extern string tokenfile;

// ����token���еĽṹ
struct Token
{
	string value1;
	string value2;
};

// �ڵ�ľ�������
struct Specificnode
{
	string dec = "";
	string stmt = "";
	string exp = "";
};

// ���ű���ṹ
struct SymbolEntry
{
	string name;	 // ��ʶ����
	string type;	 // ���ͣ�INTEGER, BOOL�ȣ�
	string procName; // ������
	int level;		 // ������㼶
	int offset;		 // �ڻ��¼�е�ƫ����
	int upperbound;
	int lowerbound;
	SymbolEntry *next; // ����ָ��

	SymbolEntry(const string &n, const string &t, int lv, int off, const string &pn, int low, int up)
		: name(n), type(t), level(lv), offset(off), next(nullptr), procName(pn), lowerbound(low), upperbound(up) {}
};

// �����﷨�������Ľڵ�
struct Treenode
{
	Treenode *child[10];
	Treenode *sibling;
	int idchild = 0;		   // ���ӵĸ���
	string nodekind;		   // �ڵ�����
	Specificnode specificnode; // ��ϸ����
	int idnum = 0;			   // ��ʶ������
	string value;			   // �ڵ��ֵ���еĽڵ�ֻ��һ����ʶ����Ҳ��value�ˣ�
	string id[10];			   // ���ֱ�ʶ����ֵ
};

// ��������﷨���ڵ�ṹ
struct Node
{
	string type;
	string name;
	string varType;
	string leftVar, rightVar;
	string isparam;
	string lowerBound = 0; // �½磨���飩
	string upperBound = 0; // �Ͻ磨���飩
	int size = 0;		   // �����С
	vector<Node *> children;
	Node *parent = nullptr; // ���ڵ�ָ��

	// Node(string t, string n = "", string vType = "", string par = "") : type(t), name(n), varType(vType), isparam(par) {}
	Node(string t, string n = "", string vType = "", string par = "", string lb = 0, string ub = 0)
		: type(t), name(n), varType(vType), isparam(par), lowerBound(lb), upperBound(ub)
	{
	}
};

// ������ķ��ű�ڵ�ṹ
struct SymbolNode
{
	string type;				  // ���͡����������̡�����
	string name;				  // ����
	int level;					  // ������㼶
	SymbolNode *child = nullptr;  // �ӽڵ�
	SymbolNode *parent = nullptr; // ���ڵ�

	SymbolNode(string t, string n = "", int i = 0) : type(t), name(n), level(i) {}
};

// ���ű�������
enum class SymKind
{
	TYPE,
	VAR,
	PROC,
	PARAM
};

// �����ṹ�����ڽṹ��/��¼�ĳ�Ա��
struct FieldEntry
{
	string name;	  // ����
	string type;	  // ����
	FieldEntry *next; // ����ָ��
	SymKind kind;

	FieldEntry(const string &n, const string &t)
		: name(n), type(t), next(nullptr) {}
};

// ���ͷ�ṹ
struct FieldChain
{
	FieldEntry *head; // ����ͷָ��
	FieldChain() : head(nullptr) {}
};

struct Quadruple
{
	string op, arg1, arg2, result;
};

// ���ű���
class SymbolTable
{
private:
	int currentLevel = 0;							 // ��ǰ������㼶
	SymbolEntry *scopeStack[100] = {nullptr};		 // ������ջ���̶�100�㣩
	SymbolEntry *currentTable = nullptr;			 // ��ǰ���ű�����ͷ
	unordered_map<string, FieldChain *> fieldTables; // ���� -> ���ӳ��

	// ����ö��ת�ַ���
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

	// �ڵ�ǰ�㼶���ҷ���
	bool SearchOneTable(const string &id, SymbolEntry **entry);

public:
	unordered_map<string, string> table;
	SymbolTable *parent;

	// ����/����
	SymbolTable(SymbolTable *p = nullptr);
	~SymbolTable();

	// ���������
	void EnterScope();
	void ExitScope();
	void CreateTable();
	void DestroyTable();

	// ���Ų���
	bool AddSymbol(const string &type, SymKind kind, const string &name);
	bool AddSymbolHead(const string &type, SymKind kind, const string &name);
	bool AddSymbolType(const string &type, SymKind kind, const string &name);
	bool AddSymbolParam(const string &procName, const string &type, SymKind kind, const string &name);
	bool AddSymbolProc(const string &type, SymKind kind, const string &name);
	bool AddSymbolArray(const string &type, SymKind kind, const string &name, int up, int low);

	// ���Ų���
	SymbolEntry *LookupCurrentScope(const string &name);
	SymbolEntry *Lookup(const string &name);
	bool FindEntry(const string &id, const string &flag, SymbolEntry **entry);
	string typereturn(const string &name);

	// ������
	void AddFieldTable(const string &typeName, FieldChain *fields);
	FieldChain *GetFieldTable(const string &typeName);
	bool FindField(const string &id, FieldChain *head, FieldEntry **entry);
	bool FindFieldInTable(const string &typeName, const string &fieldName, FieldEntry **entry);
	bool Enter(const string &id, const string &type, SymbolEntry **entry);

	// ��������
	bool insert(const string &name, const string &type, ofstream &outputFile);
	string lookup(const string &name);
	void PrintSymbolTable(ofstream &outputFile);
	void setcurrentlevel(int i);
	void addcurrentlevel(int i);
	void subcurrentlevel(int i);
};

// ȫ�ֱ���
extern int size1;
extern vector<Token> token; // token����
extern int subscript;
extern int size2;
extern int debugtest;
extern int maxlevel;
extern int currentOffset;
extern int num;							   // ��¼��������
extern int procnum;						   // ��¼���̸���
extern vector<vector<string>> paramvector; // �����ڵ�ջ
extern bool isstmtk;					   // �Ƿ������ڵ�
extern bool isassignk;					   // �Ƿ��Ǹ�ֵ�ڵ�
extern int enterdepth;					   // ��ǰ�㼶���
extern int isparam;						   // �Ƿ��ǲ����ڵ�
extern bool isconditonk;				   // �Ƿ��������ڵ�

// �ֽ��������+ | - | *| / | ( | ) | [ | ] | ; | . | < | : | = | ' | := | > | " | ,
extern char SingleDelimiter[18][20];

// �����ֵ�����,Ϊ��ʵ��ӳ���ϵ
extern string reservedWords[21];
extern string reservedWords1[21];

// ����ʷ��������ĺ���
void printwrong();
void printq(int size3, ofstream &outputFile);
void printtoken(ofstream &outputFile, vector<Token> token);
void generatetoken(string input, int len);
Token gettoken(int m);

// �����﷨�������ĺ���
Treenode *program(ofstream &outputFile);
Treenode *programhead(ofstream &outputFile);  // ����ͷ
Treenode *declarehead(ofstream &outputFile);  // ����
Treenode *typedec(ofstream &outputFile);	  // ��������
Treenode *vardec(ofstream &outputFile);		  // ��������
Treenode *procdec(ofstream &outputFile);	  // ��������
Treenode *typedeclist(ofstream &outputFile);  // ���������еĲ��ֺ���
Treenode *vardeclist(ofstream &outputFile);	  // ���������еĲ��ֺ���
Treenode *paramdeclist(ofstream &outputFile); // ���������еĲ��ֺ���
Treenode *paramlist(ofstream &outputFile);	  // ���������е��βκ���
Treenode *probody(ofstream &outputFile);	  // ���������еĺ�����,�������ֱ������������
Treenode *stmt(ofstream &outputFile);		  // ����һ�����ڵ�
Treenode *assign1(ofstream &outputFile);	  // ���ɸ�ֵ���ʽ
Treenode *write1(ofstream &outputFile);		  // ���ɶ�д���ʽ
Treenode *read1(ofstream &outputFile);		  // ���ɶ�д���ʽ
Treenode *if1(ofstream &outputFile);		  // ����ѡ����ʽ

// ��������������ĺ���
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