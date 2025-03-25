#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <vector>
#include <windows.h>
#include <fstream>
#include <stack>
#include <memory>

using namespace std;

#define distant 4  //控制*的长度

string SMLinputfile = "C:\\Users\\Fangyufan\\OneDrive\\Desktop\\test.txt";
string Lexicalfile = "C:\\Users\\Fangyufan\\OneDrive\\Desktop\\Lexical Analysis.txt";
string Syntaxfile = "C:\\Users\\Fangyufan\\OneDrive\\Desktop\\Syntax Analysis.txt";
string Semanticfile = "C:\\Users\\Fangyufan\\OneDrive\\Desktop\\Semantic Analysis.txt";
string Targetcodefile = "C:\\Users\\Fangyufan\\OneDrive\\Desktop\\Target code generation.txt";

//定义token序列的结构
struct Token {
	string value1;
	string value2;
};

//节点的具体类型
struct Specificnode {
	string dec = "";
	string stmt = "";
	string exp = "";
};

// 符号表项结构
struct SymbolEntry {
    string name;        // 标识符名
    string type;        // 类型（INTEGER, BOOL等）
    int level;          // 作用域层级
    int offset;         // 在活动记录中的偏移量
    SymbolEntry* next;  // 链表指针

    SymbolEntry(const string& n, const string& t, int lv, int off) 
        : name(n), type(t), level(lv), offset(off), next(nullptr) {}
};

//定义语法分析树的节点
struct Treenode {
	Treenode* child[10];
	Treenode* sibling;
	int idchild = 0;//儿子的个数
	string nodekind;//节点类型
	Specificnode specificnode;//详细类型
	int idnum = 0;//标识符个数
	string value;//节点的值（有的节点只有一个标识符。也用value了）
	string id[10];//部分标识符的值
};

//定义语法分析器的函数
Treenode* programhead(ofstream& outputFile); 						//程序头
Treenode* declarehead(ofstream& outputFile); 						//声明
//Treenode* programbody(); 											//程序体 
Treenode* typedec(ofstream& outputFile);  							//类型声明
Treenode* vardec(ofstream& outputFile);   							//变量声明
Treenode* procdec(ofstream& outputFile);							//过程声明
Treenode* typedeclist(ofstream& outputFile); 						//类型声明中的部分函数
Treenode* vardeclist(ofstream& outputFile); 						//变量声明中的部分函数
Treenode* paramdeclist(ofstream& outputFile); 						//过程声明中的部分函数
Treenode* paramlist(ofstream& outputFile); 							//过程声明中的形参函数
//Treenode* procdecpart(); 											//过程声明中的变量声明
Treenode* probody(ofstream& outputFile); 							//过程声明中的函数体,后面可以直接用作程序体
//Treenode* stmtlist(); 											//语句列表
Treenode* stmt(ofstream& outputFile); 								//生成一个语句节点
Treenode* assign1(ofstream& outputFile); 							//生成赋值表达式
Treenode* write1(ofstream& outputFile); 							//生成读写表达式
Treenode* read1(ofstream& outputFile); 								//生成读写表达式
Treenode* if1(ofstream& outputFile);  								//生成选择表达式
//Treenode* stmtmore(); 											//生成更多的语句节点

//全局变量
int size1 = 0;//token序列的长度
vector<Token> token;//token序列
int subscript = 0;//语法分析程序中的下标，标记读到哪个token序列
int size2 = 0;//这个用来控制树的层次结构

//分界符的命名+ | - | *| / | ( | ) | [ | ] | ; | . | < | : | = | ' | := | > | " | ,
char SingleDelimiter[18][20] = { "PLUS","MINUS","TIMES","OVER","LPAREN","RPAREN",
" LMIDPAREN","RMIDPAREN", "SEMI","DOT","LT","COLON","EQ","COMMA","ASSIGN","RT","SY","JSP1" };

// 保留字的命名,为了实现映射关系
string  reservedWords[21] = { "program","type","var","procedure","begin","end","array","of","record","if","then","else",
"fi","while","do","endwh","read","write","return","integer","char" };
string reservedWords1[21] = { "PROGRAM","TYPE","VAR","PROCEDURE","BEGIN","END","ARRAY","OF","RECORD","IF","THEN","ELSE",
"FI","WHILE","DO","ENDWH","READ","WRITE","RETURN","INTEGER","CHAR" };



//打印出错函数
void printwrong() {
	cout << "你输入的程序段有词法错误" << endl;
}

//打印语法树的空格
void printq(int size3, ofstream& outputFile) {
	for (int w = 0; w < distant * size3; w++) {
		cout << " ";
		outputFile << " ";
	}
}

//打印token序列到控制台
void printtoken(ofstream& outputFile, vector<Token>token) {
	cout << "\n\n程序经过词法分析器之后的Token序列为:" << endl;
	for (int i = 0; i < size1 - 1; i++) {
		cout << token[i].value1 << "," << token[i].value2 << endl;
		outputFile << token[i].value1 << "," << token[i].value2 << endl;
	}
	cout << token[size1 - 1].value1 << "," << token[size1 - 1].value2 << endl << "EOF";
	outputFile << token[size1 - 1].value1 << "," << token[size1 - 1].value2 << endl << "EOF";
}

//通过一个连续字符串,生成token序列,词法分析器的核心程序
void generatetoken(string input, int len) {
	int i = 0;
	while (i < len) {
		//如果遇到'\n'和空格
		if (input[i] == ' ' || input[i] == '\n') {
			i = i + 1;
			continue;
		}
		//如果遇到数字
		if (input[i] >= '0' && input[i] <= '9') {
			string tmp = "";
			tmp += input[i];
			int j = i + 1;
			while (j < len) {
				if (input[j] >= '0' && input[j] <= '9') {
					tmp += input[j];
					j++;
				}
				else {
					i = j;
					break;
				}
			}
			//下面要判断他是数学下标还是数字，这个地方的下标已经是i+1了，所以不用再变
			if (i >= len) {
				Token token1;
				token1.value1 = "NUM";
				token1.value2 = tmp;
				size1 = size1 + 1;
				token.push_back(token1);
			}
			else {
				//如果这个右边是]，则判断他是数组类型的下表
				if (input[i] == ']') {
					Token token1;
					token1.value1 = "UNDERANGE";
					token1.value2 = tmp;
					size1 = size1 + 1;
					token.push_back(token1);
				}
				else {
					Token token1;
					token1.value1 = "NUM";
					token1.value2 = tmp;
					size1 = size1 + 1;
					token.push_back(token1);
				}
			}
			continue;
		}
		//如果遇到字符
		if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z')) {
			string tmp = "";
			tmp += input[i];
			int j = i + 1;
			while (j < len) {
				if ((input[j] >= 'a' && input[j] <= 'z') || (input[j] >= 'A' && input[j] <= 'Z') || (input[j] >= '0' && input[j] <= '9')) {
					tmp += input[j];
					j = j + 1;
				}
				else {
					i = j;
					break;
				}
			}
			//cout << "\n" << "******" << tmp << endl;
			if (tmp.length() == 1) {    //如果是char类型的
				Token token1;
				token1.value1 = "CHAR";
				token1.value2 = tmp;
				size1 = size1 + 1;
				token.push_back(token1);
			}
			else {
				//判断他个是保留字
				bool flag2 = false;
				for (int k = 0; k < 21; k++) {
					if (tmp == reservedWords[k]) {
						Token token1;
						token1.value1 = "reserved word";
						token1.value2 = reservedWords1[k]; //这里将保留字的小写转变成大写
						size1 = size1 + 1;
						token.push_back(token1);
						flag2 = true;
						break;
					}
				}
				//如果他就是普通字符串
				if (!flag2) {
					Token token1;
					token1.value1 = "ID";
					token1.value2 = tmp;
					size1 = size1 + 1;
					token.push_back(token1);
				}

			}
			continue;
		}
		//如果遇到左注释和出错处理
		if (input[i] == '{') {
			int j = i; //通过双指针找到下一个}
			bool flag = false; //标识是否找到了}
			while (j < len) {
				if (input[j] == '}') {
					i = j + 1;
					flag = true;
					break;
				}
				j = j + 1;
			}
			if (!flag) {
				printwrong();
			}
			continue;
		}
		//如果遇到右注释,出错处理
		if (input[i] == '}') {
			i = i + 1;
			continue;
		}
		//如果遇到单分界符+
		if (input[i] == '+') {
			Token token1;
			token1.value1 = SingleDelimiter[0];
			token1.value2 = '+';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符-
		if (input[i] == '-') {
			Token token1;
			token1.value1 = SingleDelimiter[1];
			token1.value2 = '-';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符*
		if (input[i] == '*') {
			Token token1;
			token1.value1 = SingleDelimiter[2];
			token1.value2 = '*';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符/
		if (input[i] == '/') {
			Token token1;
			token1.value1 = SingleDelimiter[3];
			token1.value2 = '/';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符(
		if (input[i] == '(') {
			Token token1;
			token1.value1 = SingleDelimiter[4];
			token1.value2 = '(';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符)
		if (input[i] == ')') {
			Token token1;
			token1.value1 = SingleDelimiter[5];
			token1.value2 = ')';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符[
		if (input[i] == '[') {
			Token token1;
			token1.value1 = SingleDelimiter[6];
			token1.value2 = '[';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符]
		if (input[i] == ']') {
			Token token1;
			token1.value1 = SingleDelimiter[7];
			token1.value2 = ']';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符:
		if (input[i] == ';') {
			Token token1;
			token1.value1 = SingleDelimiter[8];
			token1.value2 = ';';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符.
		if (input[i] == '.') {
			Token token1;
			token1.value1 = SingleDelimiter[9];
			token1.value2 = '.';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符<
		if (input[i] == '<') {
			Token token1;
			token1.value1 = SingleDelimiter[10];
			token1.value2 = '<';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符:这个地方得考虑一下是不是:=
		if (input[i] == ':') {
			int j = i + 1;
			if (j >= len) {
				Token token1;
				token1.value1 = SingleDelimiter[11];
				token1.value2 = ':';
				size1 = size1 + 1;
				token.push_back(token1);
				i = i + 1;
			}
			else {
				if (input[j] == '=') {
					Token token1;
					token1.value1 = SingleDelimiter[14];
					token1.value2 = ":=";
					size1 = size1 + 1;
					token.push_back(token1);
					i = i + 2;
				}
				else {
					Token token1;
					token1.value1 = SingleDelimiter[11];
					token1.value2 = ':';
					size1 = size1 + 1;
					token.push_back(token1);
					i = i + 1;
				}
			}
			continue;
		}
		//如果遇到单分界符=
		if (input[i] == '=') {
			Token token1;
			token1.value1 = SingleDelimiter[12];
			token1.value2 = '=';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符'
		if (input[i] == '\'') {
			Token token1;
			token1.value1 = SingleDelimiter[13];
			token1.value2 = '\'';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符>
		if (input[i] == '>') {
			Token token1;
			token1.value1 = SingleDelimiter[15];
			token1.value2 = '>';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符"
		if (input[i] == '"') {
			Token token1;
			token1.value1 = SingleDelimiter[16];
			token1.value2 = '"';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//如果遇到单分界符,
		if (input[i] == ',') {
			Token token1;
			token1.value1 = SingleDelimiter[17];
			token1.value2 = ',';
			size1 = size1 + 1;
			token.push_back(token1);
			i = i + 1;
			continue;
		}
		//printwrong();
		i = i + 1;
	}
}

//通过下标，每次得到一个token
Token gettoken(int m) {
	return token[m];
}

//创建语法分析树的根节点
Treenode* program(ofstream& outputFile) {
	Treenode* root = new Treenode;
	if (root) {
		cout << "\n" << "程序经过语法分析器之后的语法分析树为:" << endl;
		cout << "Prok" << endl;
		outputFile << "Prok" << endl;
		root->idchild = 3;
		root->child[0] = programhead(outputFile);
		root->child[1] = declarehead(outputFile);
		root->child[2] = probody(outputFile);
		root->sibling = NULL;
		root->nodekind = "Prok";//节点类型为根节点
		if (gettoken(subscript).value1 == "DOT") {
			cout << "语法分析结果已写入: " << Syntaxfile << endl;
			return root;
		}
		else {
			return NULL;
		}
	}
}

//创建程序头头部分析函数节点
Treenode* programhead(ofstream& outputFile) {
	Treenode* tmp;
	tmp = new Treenode;
	if (tmp) {
		if (token[subscript].value2 == "PROGRAM") {
			size2 = size2 + 1;
			printq(size2, outputFile);//调用打印空格
			cout << "PheadK" << " ";
			outputFile << "PheadK" << " ";
			tmp->idchild = 0;
			tmp->nodekind = "PheadK";
			//tmp->sibling = declarehead(outputFile);
			subscript = subscript + 1;
			cout << token[subscript].value2 << endl;
			outputFile << token[subscript].value2 << endl;
			size2 = size2 - 1;//回退
			tmp->value = token[subscript].value2;
			subscript = subscript + 1;
		}
		else {
			return NULL;
		}
	}
	return tmp;
}

//创建程序声明函数节点
Treenode* declarehead(ofstream& outputFile) {
	Treenode* tmp1 = new Treenode;
	Treenode* tmp2 = new Treenode;
	Treenode* tmp3 = new Treenode;;
	Treenode* tp1 = typedec(outputFile);
	Treenode* tp2 = vardec(outputFile);
	Treenode* tp3 = procdec(outputFile);
	//对过程声明节点的细化
	if (tmp3) {
		tmp3->child[0] = tp3;
		tmp3->sibling = NULL;
		tmp3->nodekind = "Proc";
	}
	//对变量声明节点的细化
	if (tmp2) {
		tmp2->child[0] = tp2;
		tmp2->sibling = tmp3;
		tmp2->nodekind = "Varc";
	}
	//对类型声明节点的细化
	if (tmp1) {
		tmp1->child[0] = tp1;
		tmp1->nodekind = "TypeK";
		tmp1->sibling = tmp2;
	}
	//cout << 3;
	//判断不同情况得到的返回值
	if (tp1 != NULL) {
		return tmp1;
	}
	else {
		if (tp2 != NULL) {
			return tmp2;
		}
		else {
			return tmp3;//不管是不是NULL都可以这么返回
		}
	}
	return NULL;
}
//
////创建程序体函数节点
//Treenode* programbody() {
//	return NULL;
//}

//类型声明部分处理程序
Treenode* typedec(ofstream& outputFile) {
	Treenode* t = new Treenode;
	if (token[subscript].value2 == "TYPE") {
		size2 = size2 + 1;
		printq(size2, outputFile);//调用打印空格
		cout << "TYPE" << endl;
		outputFile << "TYPE" << endl;
		subscript = subscript + 1;
		//生成类型声明节点
		t->nodekind = "Typek";
		t->child[0] = typedeclist(outputFile);
		size2 = size2 - 1;//注意回退的时机
		return t;
	}
	else return NULL;
}

//变量声明处理程序
Treenode* vardec(ofstream& outputFile) {
	Treenode* t = new Treenode;
	if (token[subscript].value2 == "VAR") {
		size2 = size2 + 1;
		printq(size2, outputFile);//调用打印空格
		cout << "VAR" << endl;
		outputFile << "VAR" << endl;
		subscript = subscript + 1;
		//生成类型声明节点
		t->nodekind = "Vark";
		t->child[0] = vardeclist(outputFile);
		size2 = size2 - 1;//注意回退的时机
		return t;
	}
	else return NULL;
}

//函数声明处理程序
Treenode* procdec(ofstream& outputFile) {
	Treenode* t = new Treenode;
	if (token[subscript].value2 == "PROCEDURE") {
		size2 = size2 + 1;
		printq(size2, outputFile);//调用打印空格
		cout << "PROCEDURE" << endl;
		outputFile << "PROCEDURE" << endl;
		subscript = subscript + 1;
		//生成类型声明节点
		t->nodekind = "PROCEDURE";
		t->child[0] = paramdeclist(outputFile);
		size2 = size2 - 1;//注意回退的时机
		return t;
	}
	else return NULL;
}

//类型声明的具体函数
Treenode* typedeclist(ofstream& outputFile) {
	if (token[subscript].value2 == "VAR" || token[subscript].value2 == "PROCEDURE" ||
		token[subscript].value2 == "BEGIN" || token[subscript].value1 == "DOT") {//这里要细节一点记住value1和value2
		return NULL;
	}
	else {
		Treenode* t = new Treenode;
		if (t) {
			//生成信息
			t->value = token[subscript].value2;
			subscript = subscript + 2;
			t->nodekind = "Deck";
			t->specificnode.dec = token[subscript].value2;
			subscript = subscript + 2;
			//打印
			size2 = size2 + 1;
			printq(size2, outputFile);
			cout << t->nodekind << " " << t->specificnode.dec << " " << t->value << endl;
			outputFile << t->nodekind << " " << t->specificnode.dec << " " << t->value << endl;
			size2 = size2 - 1;//注意回退时机
			//递归调用
			t->sibling = typedeclist(outputFile);
		}
		return t;
	}
}

//变量声明的具体函数
Treenode* vardeclist(ofstream& outputFile) {
	if (token[subscript].value2 == "PROCEDURE" || token[subscript].value2 == "BEGIN" || token[subscript].value1 == "DOT") {//这里要细节一点记住value1和value2
		return NULL;
	}
	else {
		Treenode* t = new Treenode;
		if (t) {
			//生成这个节点的具体域
			t->nodekind = "Deck";
			t->specificnode.dec = token[subscript].value2;
			subscript = subscript + 1;
			while (token[subscript].value1 != "SEMI") {
				//当这个token序列中有,而不是;时
				t->id[t->idnum] = token[subscript].value2;
				t->idnum = t->idnum + 1;
				subscript = subscript + 1;
				//这里必须拉出来单独判断
				if (token[subscript].value1 == "JSP1") {
					subscript = subscript + 1;
				}
			}
			//从;到下一个token
			subscript = subscript + 1;
			//打印序列
			size2 = size2 + 1;
			printq(size2, outputFile);
			cout << "Deck" << " " << t->specificnode.dec;
			outputFile << "Deck" << " " << t->specificnode.dec;
			for (int w = 0; w < t->idnum; w++) {
				cout << " " << t->id[w];
				outputFile << " " << t->id[w];
			}
			cout << endl;
			outputFile << endl;
			size2 = size2 - 1;
			//递归调用
			t->sibling = vardeclist(outputFile);
		}
	}
}

//过程声明中的部分函数
Treenode* paramdeclist(ofstream& outputFile) {
	if (token[subscript].value1 == "ID" || token[subscript].value1 == "CHAR") {
		//生成函数名节点部分信息
		Treenode* t = new Treenode;
		t->nodekind = "HDeck";//这个地方是我自创的一个函数名节点
		t->value = token[subscript].value2;
		subscript = subscript + 1;
		//打印信息
		size2 = size2 + 1;
		printq(size2, outputFile);
		cout << "HDeck" << " " << t->value << endl;
		//生成形参，变量声明，函数体部分，作为儿子节点
		t->child[0] = paramlist(outputFile); //过程声明中的形参函数
		t->child[1] = vardec(outputFile); //过程声明中的变量声明,这个是之前写好了的
		t->child[2] = probody(outputFile); //过程声明中的函数体
		size2 = size2 - 1;
		return t;
	}
	else return NULL;
}

//过程声明中的形参函数
Treenode* paramlist(ofstream& outputFile) {
	//进去之后首先是一个左括号
	subscript = subscript + 1;
	//当有好多个形参时,生成节点
	Treenode* t = new Treenode;
	while (token[subscript].value1 != "RPAREN") {
		if (token[subscript].value1 == "JSP1") {
			//遇到,就跳过
			subscript = subscript + 1;
		}
		t->id[t->idnum] = token[subscript].value2;
		t->idnum = t->idnum + 1;
		subscript = subscript + 1;
	}
	//现在token到右括号了
	subscript = subscript + 2;
	//打印信息
	size2 = size2 + 1;
	printq(size2, outputFile);
	cout << "Deck" << " " << "value" << " " << "param";
	outputFile << "Deck" << " " << "value" << " " << "param";
	for (int w = 0; w < t->idnum; w++) {
		cout << " " << t->id[w];
		outputFile << " " << t->id[w];
	}
	cout << endl;
	outputFile << endl;
	size2 = size2 - 1;
	return t;
}

//过程声明中的函数体
Treenode* probody(ofstream& outputFile) {
	Treenode* t = new Treenode;
	if (token[subscript].value2 == "BEGIN") {
		size2 = size2 + 1;
		printq(size2, outputFile);//调用打印空格
		cout << "StmLK" << endl;
		outputFile << "StmLK" << endl;
		subscript = subscript + 1;
		//生成函数体类型声明节点
		t->nodekind = "StmLK";
		//这边进行判断，是不是语句都结束了
		while (token[subscript].value2 != "END" && token[subscript].value2 != "ENDWH") {
			t->child[t->idchild] = stmt(outputFile);
			t->idchild = t->idchild + 1;
		}
		subscript = subscript + 1;
		size2 = size2 - 1;//注意回退的时机
		return t;
	}
	else return NULL;

}

////返回语句列表
//Treenode* stmtlist() {
//	Treenode* tmp1 = new Treenode;
//	Treenode* tmp2 = new Treenode;
//	tmp1 = stmt();
//	tmp2 = stmtmore();
//	tmp1->sibling = tmp2;
//	return tmp1;
//}

//得到一个语句节点
Treenode* stmt(ofstream& outputFile) {
	if (token[subscript].value1 == "ID" || token[subscript].value1 == "CHAR") {
		//说明是赋值节点语句,生成赋值节点
		Treenode* tmp1 = new Treenode;
		tmp1->nodekind = "StmtK";
		tmp1->specificnode.stmt = "AssignK";
		//打印这个
		size2 = size2 + 1;
		printq(size2, outputFile);
		//这边加一些判断来区分函数调用和赋值语句
		if (token[subscript + 1].value1 == "LPAREN") {
			cout << "StmtK" << " " << "CALL" << endl;
			outputFile << "StmtK" << " " << "CALL" << endl;
		}
		else {
			cout << "StmtK" << " " << "AssignK" << endl;
			outputFile << "StmtK" << " " << "AssignK" << endl;
		}
		//cout << "StmtK" << " " << "AssignK" << endl;
		//outputFile << "StmtK" << " " << "AssignK" << endl;
		//继续生成赋值节点细的部分；
		tmp1->child[0] = assign1(outputFile);
		size2 = size2 - 1;
		return tmp1;
	}
	else {
		if (token[subscript].value2 == "WRITE") {
			//说明是写出节点
			Treenode* tmp1 = new Treenode;
			tmp1->nodekind = "StmtK";
			tmp1->specificnode.stmt = "WRITE";
			subscript = subscript + 1;
			//打印这个
			size2 = size2 + 1;
			printq(size2, outputFile);
			cout << "StmtK" << " " << "WRITE" << endl;
			outputFile << "StmtK" << " " << "WRITE" << endl;
			//继续生成写节点细的部分；
			tmp1->child[0] = write1(outputFile);
			size2 = size2 - 1;
			return tmp1;
		}
		else {
			if (token[subscript].value2 == "READ") {
				//说明是读入节点
				Treenode* tmp1 = new Treenode;
				tmp1->nodekind = "StmtK";
				tmp1->specificnode.stmt = "READ";
				subscript = subscript + 1;
				//打印这个
				size2 = size2 + 1;
				printq(size2, outputFile);
				cout << "StmtK" << " " << "READ" << endl;
				outputFile << "StmtK" << " " << "READ" << endl;
				//继续生成读节点细的部分；
				tmp1->child[0] = read1(outputFile);
				size2 = size2 - 1;
				return tmp1;
			}
			else {
				if (token[subscript].value2 == "IF") {
					//说明是判断节点
					Treenode* tmp1 = new Treenode;
					tmp1->nodekind = "StmtK";
					tmp1->specificnode.stmt = "IF";
					subscript = subscript + 1;
					size2 = size2 + 1;
					printq(size2, outputFile);
					cout << "StmtK" << " " << "IF" << endl;
					outputFile << "StmtK" << " " << "IF" << endl;
					//继续生成选择节点if判断部分；
					tmp1->child[0] = if1(outputFile);
					//打印then部分
					size2 = size2 + 1;
					printq(size2, outputFile);
					cout << "StmtK" << " " << "THEN" << endl;
					outputFile << "StmtK" << " " << "THEN" << endl;
					subscript = subscript + 1;
					tmp1->child[2] = assign1(outputFile);
					size2 = size2 - 1;
					//如果有else
					if (token[subscript].value2 == "ELSE") {
						size2 = size2 + 1;
						printq(size2, outputFile);
						cout << "StmtK" << " " << "ELSE" << endl;
						outputFile << "StmtK" << " " << "ELSE" << endl;
						subscript = subscript + 1;
						tmp1->child[3] = assign1(outputFile);
						size2 = size2 - 1;
					}
					//对于FI的处理
					subscript = subscript + 2;
					//别忘了一开始的if判断
					size2 = size2 - 1;
					return tmp1;

				}
				else {
					subscript = subscript + 1;
					return NULL;
				}
			}
		}

	}
}

//生成赋值表达式节点
Treenode* assign1(ofstream& outputFile) {
	//想的简单一点，就遇到;就停止
	Treenode* tmp1 = new Treenode;
	while (token[subscript].value1 != "SEMI" && token[subscript].value2 != "END" && token[subscript].value2 != "ENDWH"
		&& token[subscript].value2 != "ELSE" && token[subscript].value2 != "FI") {
		if (token[subscript].value1 == "JSP1" || token[subscript].value1 == "ASSIGN" || token[subscript].value1 == "EQ"
			|| token[subscript].value1 == "LPAREN" || token[subscript].value1 == "RPAREN" || token[subscript].value1 == "SY"
			|| token[subscript].value1 == "COMMA") {
			subscript = subscript + 1;
		}
		else {
			tmp1->nodekind = "ExpK";
			tmp1->specificnode.stmt = "IdK";
			size2 = size2 + 1;
			printq(size2, outputFile);
			//这边对赋值语句的不同符号进行区分
			//加减乘除
			if ((token[subscript].value1 == "PLUS") || (token[subscript].value1 == "MINUS") || (token[subscript].value1 == "TIMES")
				|| (token[subscript].value1 == "OVER")) {
				cout << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
				outputFile << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
			}
			else {
				//整数
				if (token[subscript].value1 == "NUM") {
					cout << "ExpK" << " " << "const" << " " << token[subscript].value2 << endl;
					outputFile << "ExpK" << " " << "const" << " " << token[subscript].value2 << endl;
				}
				else {
					cout << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
					outputFile << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
				}
				//cout << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
				//outputFile << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
			}
			subscript = subscript + 1;
			size2 = size2 - 1;
		}
		//tmp1->nodekind = "ExpK";
		//tmp1->specificnode.stmt = "IdK";
		//size2 = size2 + 1;
		//printq(size2, outputFile);
		//if ((token[subscript].value1 == "PLUS") || (token[subscript].value1 == "MINUS") || (token[subscript].value1 == "TIMES")
		//	|| (token[subscript].value1 == "OVER")) {
		//	cout << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
		//}
		//else {
		//	cout << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
		//}
		//subscript = subscript + 1;
		//size2 = size2 - 1;
	}
	if (token[subscript].value1 == "SEMI") {
		subscript = subscript + 1;
	}
	return tmp1;
}

//生成写表达式
Treenode* write1(ofstream& outputFile) {
	Treenode* tmp1 = new Treenode;
	while ((token[subscript].value1 != "SEMI" && token[subscript].value2 != "END" && token[subscript].value2 != "ENDWH")) {
		if (token[subscript].value1 == "LPAREN" || token[subscript].value1 == "RPAREN") {
			subscript = subscript + 1;
		}
		else {
			tmp1->nodekind = "ExpK";
			tmp1->specificnode.stmt = "IdK";
			size2 = size2 + 1;
			printq(size2, outputFile);
			cout << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
			outputFile << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
			subscript = subscript + 1;
			size2 = size2 - 1;
		}
	}
	if (token[subscript].value1 == "SEMI") {
		subscript = subscript + 1;
	}
	return tmp1;

}

//生成读表达式
Treenode* read1(ofstream& outputFile) {
	Treenode* tmp1 = new Treenode;
	while ((token[subscript].value1 != "SEMI" && token[subscript].value2 != "END" && token[subscript].value2 != "ENDWH")) {
		if (token[subscript].value1 == "LPAREN" || token[subscript].value1 == "RPAREN") {
			subscript = subscript + 1;
		}
		else {
			tmp1->nodekind = "ExpK";
			tmp1->specificnode.stmt = "IdK";
			size2 = size2 + 1;
			printq(size2, outputFile);
			cout << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
			outputFile << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
			subscript = subscript + 1;
			size2 = size2 - 1;
		}
	}
	if (token[subscript].value1 == "SEMI") {
		subscript = subscript + 1;
	}
	return tmp1;
}

//生成if表达式
Treenode* if1(ofstream& outputFile) {
	Treenode* tmp1 = new Treenode;
	while (token[subscript].value2 != "THEN") {
		if (token[subscript].value1 == "LPAREN" || token[subscript].value1 == "RPAREN") {
			subscript = subscript + 1;
		}
		else {
			tmp1->nodekind = "ExpK";
			size2 = size2 + 1;
			printq(size2, outputFile);
			//这边对赋值语句的不同符号进行区分
			//加减乘除
			if ((token[subscript].value1 == "PLUS") || (token[subscript].value1 == "MINUS") || (token[subscript].value1 == "TIMES")
				|| (token[subscript].value1 == "OVER") || (token[subscript].value1 == "LT") || (token[subscript].value1 == "RT")) {
				cout << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
				outputFile << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
			}
			else {
				//整数
				if (token[subscript].value1 == "NUM") {
					cout << "ExpK" << " " << "const" << " " << token[subscript].value2 << endl;
					outputFile << "ExpK" << " " << "const" << " " << token[subscript].value2 << endl;
				}
				else {
					cout << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
					outputFile << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
				}
				//cout << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
				//outputFile << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
			}
			subscript = subscript + 1;
			size2 = size2 - 1;
		}
	}
	return tmp1;
}

// 符号表项类型
enum class SymKind { TYPE, VAR, PROC, PARAM };



// 语法树节点结构
struct Node {
	string type;
	string name;
	string varType;
	string leftVar, rightVar;
	vector<Node*> children;

	Node(string t, string n = "", string vType = "") : type(t), name(n), varType(vType) {}
};

// 域表项结构（用于结构体/记录的成员）
struct FieldEntry {
    string name;          // 域名
    string type;          // 类型
    FieldEntry* next;     // 链表指针
    SymKind kind;

    FieldEntry(const string& n, const string& t) 
        : name(n), type(t), next(nullptr) {}
};

// 域表头结构
struct FieldChain {
    FieldEntry* head;     // 链表头指针
    FieldChain() : head(nullptr) {}
};

// 符号表（支持作用域）
class SymbolTable {
private:
	int currentLevel = 0;                   	// 当前作用域层级
	int currentOffset = 7;                  	// 初始偏移量（保留7个位置）
	SymbolEntry* scopeStack[100] = {nullptr}; 	// 作用域栈（假设最多100层）
	SymbolEntry* currentTable = nullptr;     	// 当前符号表链表头
	unordered_map<string, FieldChain*> fieldTables; // 域名 -> 域表映射
    string KindToString(SymKind kind) {
        switch (kind) {
            case SymKind::TYPE: return "TYPE";
            case SymKind::VAR: return "VAR";
            case SymKind::PROC: return "PROC";
            case SymKind::PARAM: return "PARAM";
            default: return "UNKNOWN";
        }
    }
public:
	unordered_map<string, string> table;
	SymbolTable* parent;

	SymbolTable(SymbolTable* p = nullptr) : parent(p) {}

	bool insert(const string& name, const string& type, ofstream& outputFile) {
		if (table.find(name) != table.end()) {
			cout << "Error: Variable '" << name << "' redeclared in the same scope." << endl;
			outputFile << "Error: Variable '" << name << "' redeclared in the same scope." << endl;
			return false;
		}
		table[name] = type;
		return true;
	}

	string lookup(const string& name) {
		if (table.find(name) != table.end()) return table[name];
		if (parent) return parent->lookup(name);
		return "ERROR: Undeclared variable '" + name + "'";
	}

	// 进入新作用域
	void CreateTable() {
		currentLevel++;
		currentOffset = 7;  // 重置偏移量
		scopeStack[currentLevel] = nullptr; // 初始化新层符号表
	}

	// 退出当前作用域
	void DestroyTable() {
		// 释放当前层符号表内存
		SymbolEntry* p = scopeStack[currentLevel];
		while (p) {
			SymbolEntry* tmp = p;
			p = p->next;
			delete tmp;
		}
		currentLevel--;
	}

    // 进入新作用域
    void EnterScope() {
        currentLevel++;
        scopeStack[currentLevel] = nullptr;
    }

    // 退出当前作用域
    void ExitScope() {
        SymbolEntry* p = scopeStack[currentLevel];
        while (p) {
            SymbolEntry* tmp = p;
            p = p->next;
            delete tmp;
        }
        currentLevel--;
    }

	// 在当前符号表查找标识符
	bool SearchOneTable(const string& id, SymbolEntry** entry) {
		SymbolEntry* p = scopeStack[currentLevel];
		while (p) {
			if (p->name == id) {
				if (entry) *entry = p;
				return true;
			}
			p = p->next;
		}
		return false;
	}

	// 跨作用域查找标识符（对应FindEntry，flag控制方向）
	bool FindEntry(const string& id, const string& flag, SymbolEntry** entry) {
		if (flag == "one") {
			return SearchOneTable(id, entry); // 仅查当前表
		}

		// 向上查找（up）
		if (flag == "up") {
			for (int lv = currentLevel; lv >= 0; lv--) {
				SymbolEntry* p = scopeStack[lv];
				while (p) {
					if (p->name == id) {
						if (entry) *entry = p;
						return true;
					}
					p = p->next;
				}
			}
		} 
		// 向下查找（down）
		else if (flag == "down") {
			for (int lv = currentLevel; lv < 100 && scopeStack[lv]; lv++) {
				SymbolEntry* p = scopeStack[lv];
				while (p) {
					if (p->name == id) {
						if (entry) *entry = p;
						return true;
					}
					p = p->next;
				}
			}
		}
		return false;
	}

	// 登记标识符（对应Enter函数）
	bool Enter(const string& id, const string& type, SymbolEntry** entry) {
		// 检查重复声明
		if (SearchOneTable(id, nullptr)) {
			cerr << "Error: Duplicate declaration of '" << id << "' in scope level " << currentLevel << endl;
			return false;
		}

		// 创建新符号表项
		SymbolEntry* newEntry = new SymbolEntry(id, type, currentLevel, currentOffset);
		currentOffset++; // 更新偏移量

		// 插入链表头部
		newEntry->next = scopeStack[currentLevel];
		scopeStack[currentLevel] = newEntry;

		if (entry) *entry = newEntry;
		return true;
	}

	//打印符号表
	void PrintSymbolTable() {
		cout << "===== Symbol Table (Current Level: " << currentLevel << ") =====" << endl;
		for (int lv = currentLevel; lv >= 0; lv--) {
			cout << "--- Level " << lv << " ---" << endl;
			SymbolEntry* p = scopeStack[lv];
			while (p) {
				cout << "Name: " << p->name << " | Type: " << p->type 
						<< " | Offset: " << p->offset << endl;
				p = p->next;
			}
		}
	}

	bool FindField(const string& id, FieldChain* head, FieldEntry** entry) {
		if (!head || !head->head) {
			if (entry) *entry = nullptr;
			return false;
		}
	
		FieldEntry* p = head->head;
		while (p) {
			if (p->name == id) {
				if (entry) *entry = p;
				return true;
			}
			p = p->next;
		}
	
		if (entry) *entry = nullptr;
		return false;
	}

	// 添加域表（用于结构体/记录类型）
	void AddFieldTable(const string& typeName, FieldChain* fields) {
		fieldTables[typeName] = fields;
	}

	// 查找域表
	FieldChain* GetFieldTable(const string& typeName) {
		return fieldTables.count(typeName) ? fieldTables[typeName] : nullptr;
	}

	// 域表查找（封装FindField）
	bool FindFieldInTable(const string& typeName, const string& fieldName, FieldEntry** entry) {
		FieldChain* table = GetFieldTable(typeName);
		return FindField(fieldName, table, entry);
	}

	~SymbolTable() {
		while (currentLevel >= 0) {
			DestroyTable();
		}
	}
};

// 解析语法树
Node* parseSyntaxTree(const string& filePath) {
    ifstream file(filePath);
    if (!file) {
        cerr << "Error: Cannot open file " << filePath << endl;
        return nullptr;
    }

    stack<pair<int, Node*>> nodeStack; // 维护层级和节点
    Node* root = nullptr;
    string line;

    while (getline(file, line)) {
        int indent = 0;
        while (indent < line.size() && (line[indent] == ' ' || line[indent] == '\t')) {
            indent++; // 计算缩进级别
        }
        line = line.substr(indent); // 去掉缩进空格

        if (line.empty()) continue; // 跳过空行

        istringstream iss(line);
        string type, name, varType;
        iss >> type >> name >> varType; // 读取类型、变量名、变量类型

        Node* newNode = new Node(type, name, varType);

        if (nodeStack.empty()) {
            root = newNode; // 第一个节点作为根节点
        } else {
            // 维护正确的层级关系
            while (!nodeStack.empty() && nodeStack.top().first >= indent) {
                nodeStack.pop();
            }
            if (!nodeStack.empty()) {
                nodeStack.top().second->children.push_back(newNode);
            }
        }

        nodeStack.push({indent, newNode});
    }

    file.close();
    return root;
}


// 语义分析函数
void semanticAnalysis(Node* tree, SymbolTable* symTable, ofstream& outputFile) {
	if (!tree) return;

	// 处理类型和变量声明
	if (tree->type == "TYPE" || tree->type == "VAR") {
		for (auto child : tree->children) {
			if (!symTable->insert(child->name, child->varType, outputFile)) {
				outputFile << "Error: Variable '" << child->name << "' redeclared in the same scope." << endl;
			}
		}
	}

	// 处理过程声明
	if (tree->type == "PROCEDURE") {
		SymbolTable* newScope = new SymbolTable(symTable);
		for (auto child : tree->children) {
			semanticAnalysis(child, newScope, outputFile);
		}
	}

	// 处理语句块
	if (tree->type == "StmLK") {
		for (auto stmt : tree->children) {
			if (stmt->type == "READ" || stmt->type == "WRITE") {
				string res = symTable->lookup(stmt->name);
				if (res.find("ERROR") != string::npos) {
					outputFile << res << endl;
				}
			}

			// 赋值语句处理
			if (stmt->type == "AssignK") {
				string leftType = symTable->lookup(stmt->leftVar);
				string rightType = symTable->lookup(stmt->rightVar);

				if (leftType.find("ERROR") != string::npos) {
					outputFile << leftType << endl;
				}
				if (rightType.find("ERROR") != string::npos) {
					outputFile << rightType << endl;
				}
				if (leftType != rightType && leftType.find("ERROR") == string::npos && rightType.find("ERROR") == string::npos) {
					outputFile << "Error: Type mismatch in assignment of '" << stmt->leftVar << "'." << endl;
				}
			}

			// 条件语句处理
			if (stmt->type == "IF" || stmt->type == "WHILE") {
				string condType = symTable->lookup(stmt->children[0]->name);
				if (condType != "BOOLEAN") {
					outputFile << "Error: Condition expression must be of BOOLEAN type." << endl;
				}
			}

			// 过程调用处理
			if (stmt->type == "CALL") {
				string procType = symTable->lookup(stmt->name);
				if (procType != "PROCEDURE") {
					outputFile << "Error: Identifier '" << stmt->name << "' is not a procedure." << endl;
				}
			}
		}
	}

	// 递归遍历子节点
	for (auto child : tree->children) {
		semanticAnalysis(child, symTable, outputFile);
	}
}

//检查语法树
void printSyntaxTree(Node* node, int depth = 0) {
    if (!node) return;

    // 根据层级深度添加缩进
    for (int i = 0; i < depth; ++i) {
        cout << "    "; // 每层缩进4个空格
    }

    // 打印当前节点信息
    cout << node->type;
    if (!node->name.empty()) cout << " (" << node->name << ")";
    if (!node->varType.empty()) cout << " : " << node->varType;
    cout << endl;

    // 递归打印子节点
    for (auto child : node->children) {
        printSyntaxTree(child, depth + 1);
    }
}

//检查符号表（test）
void printSymbolTable(SymbolTable* table, int depth = 0) {
    if (!table) return;

    cout << string(depth * 4, ' ') << "===== Symbol Table (Scope Depth: " << depth << ") =====" << endl;
    for (const auto& entry : table->table) {
        cout << string(depth * 4, ' ') << "Name: " << entry.second << " | Type: " << entry.first << endl;
    }

    // 递归打印父作用域的符号表
    if (table->parent) {
        printSymbolTable(table->parent, depth + 1);
    }
}

////得到更多的语句节点
//Treenode* stmtmore() {
//	if (token[subscript].value2 == "END" || token[subscript].value2 == "ENDWH") {
//		subscript = subscript + 1;
//		return NULL;
//	}
//	else {
//
//	}
//}

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

	//语义分析
	Node* syntaxTree = parseSyntaxTree(Syntaxfile);
	SymbolTable* symTable = new SymbolTable();

	cout << "\n程序经过语义分析器之后的语义错误信息为和符号表为:" << endl;
	cout << "Semantic Analysis Results:" << endl;
	semanticAnalysis(syntaxTree, symTable, Semantic);
	cout << "\nSymbol Table:" << endl;
    symTable->PrintSymbolTable();
	cout << "语义错误信息已写入: " << Semanticfile << endl;
	
	//目标代码生成

	Lexical.close();
	Syntax.close();
	Semantic.close();
	Targetcode.close();

	{
		// 符号表测试
		cout << endl << endl << endl << "Symbol Table Test:" << endl;
		SymbolTable symTablex;

		// 进入全局作用域
		symTablex.CreateTable();
		symTablex.Enter("x", "INTEGER", nullptr);
		symTablex.Enter("y", "BOOL", nullptr);

		// 进入局部作用域
		symTablex.CreateTable();
		symTablex.Enter("i", "INTEGER", nullptr);
		symTablex.Enter("x", "REAL", nullptr); // 错误：重复声明

		symTablex.PrintSymbolTable();
		cout << endl;

		// 查找测试
		SymbolEntry* entry;
		if (symTablex.FindEntry("y", "up", &entry)) {
			cout << "Found y in level " << entry->level << endl;
		}

		// 退出局部作用域
		symTablex.DestroyTable();
	}

	return 0;
}