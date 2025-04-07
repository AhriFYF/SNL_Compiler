#include "main.h"
#define MAX_LINE_LEN 256  // 单行最大长度
#define MAX_FIELD_LEN 128 // 字段最大长度

void printq(int size3, ofstream &outputFile)
{
	for (int w = 0; w < distant * size3; w++)
	{
		cout << " ";
		outputFile << " ";
	}
}

Token gettoken(int m)
{
	return token[m];
}
// 创建语法分析树的根节点
Treenode *program(ofstream &outputFile)
{
	Treenode *root = new Treenode;
	if (root)
	{
		cout << "\n"
			 << "程序经过语法分析器之后的语法分析树为:" << endl;
		cout << "Prok" << endl;

		outputFile << "Prok" << endl;
		root->idchild = 3;
		root->child[0] = programhead(outputFile); // 创建程序头头部分析函数节点
		root->child[1] = declarehead(outputFile); // 创建程序声明函数节点
		root->child[2] = probody(outputFile);	  // 过程声明中的函数体
		root->sibling = NULL;
		root->nodekind = "Prok"; // 节点类型为根节点
		if (gettoken(subscript).value1 == "DOT")
		{
			return root;
		}
		else
		{
			return NULL;
		}
	}
}

// 创建程序头头部分析函数节点
Treenode *programhead(ofstream &outputFile)
{
	Treenode *tmp;
	tmp = new Treenode;
	if (tmp)
	{
		if (token[subscript].value2 == "PROGRAM")
		{
			size2 = size2 + 1;
			printq(size2, outputFile); // 调用打印空格
			cout << "PheadK" << " ";
			outputFile << "PheadK" << " ";
			tmp->idchild = 0;
			tmp->nodekind = "PheadK";
			// tmp->sibling = declarehead(outputFile);
			subscript = subscript + 1;
			cout << token[subscript].value2 << endl;
			outputFile << token[subscript].value2 << endl;
			size2 = size2 - 1; // 回退
			tmp->value = token[subscript].value2;
			subscript = subscript + 1;
		}
		else
		{
			return NULL;
		}
	}
	return tmp;
}

// 创建程序声明函数节点
Treenode *declarehead(ofstream &outputFile)
{
	Treenode *tmp1 = new Treenode;
	Treenode *tmp2 = new Treenode;
	Treenode *tmp3 = new Treenode;
	;
	Treenode *tp1 = typedec(outputFile);
	Treenode *tp2 = vardec(outputFile);
	Treenode *tp3 = procdec(outputFile);
	// 对过程声明节点的细化
	if (tmp3)
	{
		tmp3->child[0] = tp3;
		tmp3->sibling = NULL;
		tmp3->nodekind = "Proc";
	}
	// 对变量声明节点的细化
	if (tmp2)
	{
		tmp2->child[0] = tp2;
		tmp2->sibling = tmp3;
		tmp2->nodekind = "Varc";
	}
	// 对类型声明节点的细化
	if (tmp1)
	{
		tmp1->child[0] = tp1;
		tmp1->nodekind = "TypeK";
		tmp1->sibling = tmp2;
	}
	// cout << 3;
	// 判断不同情况得到的返回值
	if (tp1 != NULL)
	{
		return tmp1;
	}
	else
	{
		if (tp2 != NULL)
		{
			return tmp2;
		}
		else
		{
			return tmp3; // 不管是不是NULL都可以这么返回
		}
	}
	return NULL;
}
//
////创建程序体函数节点
// Treenode* programbody() {
//	return NULL;
// }

// 类型声明部分处理程序
Treenode *typedec(ofstream &outputFile)
{
	Treenode *t = new Treenode;
	if (token[subscript].value2 == "TYPE")
	{
		size2 = size2 + 1;
		printq(size2, outputFile); // 调用打印空格
		cout << "TYPE" << endl;
		outputFile << "TYPE" << endl;
		subscript = subscript + 1;
		// 生成类型声明节点
		t->nodekind = "Typek";
		t->child[0] = typedeclist(outputFile);
		size2 = size2 - 1; // 注意回退的时机
		return t;
	}
	else
		return NULL;
}

// 变量声明处理程序
Treenode *vardec(ofstream &outputFile)
{
	Treenode *t = new Treenode;
	if (token[subscript].value2 == "VAR")
	{
		size2 = size2 + 1;
		printq(size2, outputFile); // 调用打印空格
		cout << "VAR" << endl;
		outputFile << "VAR" << endl;
		subscript = subscript + 1;
		// 生成类型声明节点
		t->nodekind = "Vark";
		t->child[0] = vardeclist(outputFile);
		size2 = size2 - 1; // 注意回退的时机
		return t;
	}
	else
		return NULL;
}

// 函数声明处理程序
Treenode *procdec(ofstream &outputFile)
{
	Treenode *t = new Treenode;
	if (token[subscript].value2 == "PROCEDURE")
	{
		size2 = size2 + 1;
		printq(size2, outputFile); // 调用打印空格
		cout << "PROCEDURE" << endl;
		outputFile << "PROCEDURE" << endl;
		subscript = subscript + 1;
		// 生成类型声明节点
		t->nodekind = "PROCEDURE";
		t->child[0] = paramdeclist(outputFile);
		size2 = size2 - 1; // 注意回退的时机
		return t;
	}
	else
		return NULL;
}

// 类型声明的具体函数
Treenode *typedeclist(ofstream &outputFile)
{
	if (token[subscript].value2 == "VAR" || token[subscript].value2 == "PROCEDURE" ||
		token[subscript].value2 == "BEGIN" || token[subscript].value1 == "DOT")
	{ // 这里要细节一点记住value1和value2
		return NULL;
	}
	else
	{
		Treenode *t = new Treenode;
		if (t)
		{
			// 生成信息
			t->value = token[subscript].value2;
			subscript = subscript + 2;
			t->nodekind = "Deck";
			t->specificnode.dec = token[subscript].value2;
			subscript = subscript + 2;
			// 打印
			size2 = size2 + 1;
			printq(size2, outputFile);
			cout << t->nodekind << " " << t->specificnode.dec << " " << t->value << endl;
			outputFile << t->nodekind << " " << t->specificnode.dec << " " << t->value << endl;
			size2 = size2 - 1; // 注意回退时机
			// 递归调用
			t->sibling = typedeclist(outputFile);
		}
		return t;
	}
}

// 变量声明的具体函数
Treenode *vardeclist(ofstream &outputFile)
{
	if (token[subscript].value2 == "PROCEDURE" || token[subscript].value2 == "BEGIN" || token[subscript].value1 == "DOT")
	{ // 这里要细节一点记住value1和value2
		return NULL;
	}
	else
	{
		if (token[subscript].value2 == "VAR")
			subscript = subscript + 1;
		Treenode *t = new Treenode;
		if (t)
		{
			// 生成这个节点的具体域
			t->nodekind = "Deck";
			t->specificnode.dec = token[subscript].value2;
			subscript = subscript + 1;
			while (token[subscript].value1 != "SEMI")
			{
				// 当这个token序列中有,而不是;时
				t->id[t->idnum] = token[subscript].value2;
				t->idnum = t->idnum + 1;
				subscript = subscript + 1;
				// 这里必须拉出来单独判断
				if (token[subscript].value1 == "JSP1")
				{
					subscript = subscript + 1;
				}
			}
			// 从;到下一个token
			subscript = subscript + 1;
			// 打印序列
			size2 = size2 + 1;
			printq(size2, outputFile);
			cout << "Deck" << " " << t->specificnode.dec;
			outputFile << "Deck" << " " << t->specificnode.dec;
			for (int w = 0; w < t->idnum; w++)
			{
				cout << " " << t->id[w];
				outputFile << " " << t->id[w];
			}
			cout << endl;
			outputFile << endl;
			size2 = size2 - 1;
			// 递归调用
			t->sibling = vardeclist(outputFile);
		}
	}
}

// 过程声明中的部分函数
Treenode *paramdeclist(ofstream &outputFile)
{
	if (token[subscript].value1 == "ID" || token[subscript].value1 == "CHAR")
	{
		// 生成函数名节点部分信息
		Treenode *t = new Treenode;
		t->nodekind = "HDeck"; // 自创的一个函数名节点
		t->value = token[subscript].value2;
		subscript = subscript + 1;
		// 打印信息
		size2 = size2 + 1;
		printq(size2, outputFile);
		cout << "HDeck" << " " << t->value << endl;
		outputFile << "HDeck" << " " << t->value << endl;
		// 生成形参，变量声明，函数体部分，作为儿子节点
		t->child[0] = paramlist(outputFile); // 过程声明中的形参函数
		t->child[1] = vardec(outputFile);	 // 过程声明中的变量声明
		t->child[2] = probody(outputFile);	 // 过程声明中的函数体
		size2 = size2 - 1;
		return t;
	}
	else
		return NULL;
}

// 过程声明中的形参函数
Treenode *paramlist(ofstream &outputFile)
{
	// 进去之后首先是一个左括号
	subscript = subscript + 1;
	// 当有好多个形参时,生成节点
	Treenode *t = new Treenode;
	while (token[subscript].value1 != "RPAREN")
	{
		if (token[subscript].value1 == "JSP1")
		{
			// 遇到,就跳过
			subscript = subscript + 1;
		}
		t->id[t->idnum] = token[subscript].value2;
		t->idnum = t->idnum + 1;
		subscript = subscript + 1;
	}
	// 现在token到右括号了
	subscript = subscript + 2;
	// 打印信息
	size2 = size2 + 1;
	printq(size2, outputFile);
	cout << "Deck" << " " << "value" << " " << "param";
	outputFile << "Deck" << " " << "value" << " " << "param";
	for (int w = 0; w < t->idnum; w++)
	{
		cout << " " << t->id[w];
		outputFile << " " << t->id[w];
	}
	cout << endl;
	outputFile << endl;
	size2 = size2 - 1;
	return t;
}

// 过程声明中的函数体
Treenode *probody(ofstream &outputFile)
{
	Treenode *t = new Treenode;
	if (token[subscript].value2 == "BEGIN")
	{
		size2 = size2 + 1;
		printq(size2, outputFile); // 调用打印空格
		cout << "StmLK" << endl;
		outputFile << "StmLK" << endl;
		subscript = subscript + 1;
		// 生成函数体类型声明节点
		t->nodekind = "StmLK";
		// 这边进行判断，是不是语句都结束了
		while (token[subscript].value2 != "END" && token[subscript].value2 != "ENDWH")
		{
			t->child[t->idchild] = stmt(outputFile);
			t->idchild = t->idchild + 1;
		}
		subscript = subscript + 1;
		size2 = size2 - 1; // 注意回退的时机
		return t;
	}
	else
		return NULL;
}

// 得到一个语句节点
Treenode *stmt(ofstream &outputFile)
{
	if (token[subscript].value1 == "ID" || token[subscript].value1 == "CHARC" || token[subscript].value1 == "INTC")
	{
		// 说明是赋值节点语句,生成赋值节点
		Treenode *tmp1 = new Treenode;
		tmp1->nodekind = "StmtK";
		tmp1->specificnode.stmt = "AssignK";
		// 打印这个
		size2 = size2 + 1;
		printq(size2, outputFile);
		// 这边加一些判断来区分函数调用和赋值语句
		if (token[subscript + 1].value1 == "LPAREN")
		{
			cout << "StmtK" << " " << "CALL" << endl;
			outputFile << "StmtK" << " " << "CALL" << endl;
		}
		else
		{
			cout << "StmtK" << " " << "AssignK" << endl;
			outputFile << "StmtK" << " " << "AssignK" << endl;
		}
		// cout << "StmtK" << " " << "AssignK" << endl;
		// outputFile << "StmtK" << " " << "AssignK" << endl;
		// 继续生成赋值节点细的部分；
		tmp1->child[0] = assign1(outputFile);
		size2 = size2 - 1;
		return tmp1;
	}
	else
	{
		if (token[subscript].value2 == "WRITE")
		{
			// 说明是写出节点
			Treenode *tmp1 = new Treenode;
			tmp1->nodekind = "StmtK";
			tmp1->specificnode.stmt = "WRITE";
			subscript = subscript + 1;
			// 打印这个
			size2 = size2 + 1;
			printq(size2, outputFile);
			cout << "StmtK" << " " << "WRITE" << endl;
			outputFile << "StmtK" << " " << "WRITE" << endl;
			// 继续生成写节点细的部分；
			tmp1->child[0] = write1(outputFile);
			size2 = size2 - 1;
			return tmp1;
		}
		else
		{
			if (token[subscript].value2 == "READ")
			{
				// 说明是读入节点
				Treenode *tmp1 = new Treenode;
				tmp1->nodekind = "StmtK";
				tmp1->specificnode.stmt = "READ";
				subscript = subscript + 1;
				// 打印这个
				size2 = size2 + 1;
				printq(size2, outputFile);
				cout << "StmtK" << " " << "READ" << endl;
				outputFile << "StmtK" << " " << "READ" << endl;
				// 继续生成读节点细的部分；
				tmp1->child[0] = read1(outputFile);
				size2 = size2 - 1;
				return tmp1;
			}
			else
			{
				if (token[subscript].value2 == "IF")
				{
					// 说明是判断节点
					Treenode *tmp1 = new Treenode;
					tmp1->nodekind = "StmtK";
					tmp1->specificnode.stmt = "IF";
					subscript = subscript + 1;
					size2 = size2 + 1;
					printq(size2, outputFile);
					cout << "StmtK" << " " << "IF" << endl;
					outputFile << "StmtK" << " " << "IF" << endl;
					// 继续生成选择节点if判断部分；
					tmp1->child[0] = if1(outputFile);
					// 打印then部分
					size2 = size2 + 1;
					printq(size2, outputFile);
					cout << "StmtK" << " " << "THEN" << endl;
					outputFile << "StmtK" << " " << "THEN" << endl;
					subscript = subscript + 1;
					tmp1->child[2] = assign1(outputFile);
					size2 = size2 - 1;
					// 如果有else
					if (token[subscript].value2 == "ELSE")
					{
						size2 = size2 + 1;
						printq(size2, outputFile);
						cout << "StmtK" << " " << "ELSE" << endl;
						outputFile << "StmtK" << " " << "ELSE" << endl;
						subscript = subscript + 1;
						tmp1->child[3] = assign1(outputFile);
						size2 = size2 - 1;
					}
					// 对于FI的处理
					subscript = subscript + 2;
					// 别忘了一开始的if判断
					size2 = size2 - 1;
					return tmp1;
				}
				else
				{
					subscript = subscript + 1;
					return NULL;
				}
			}
		}
	}
}

// 生成赋值表达式节点
Treenode *assign1(ofstream &outputFile)
{
	// 想的简单一点，就遇到;就停止
	Treenode *tmp1 = new Treenode;
	while (token[subscript].value1 != "SEMI" && token[subscript].value2 != "END" && token[subscript].value2 != "ENDWH" && token[subscript].value2 != "ELSE" && token[subscript].value2 != "FI")
	{
		if (token[subscript].value1 == "JSP1" || token[subscript].value1 == "ASSIGN" || token[subscript].value1 == "EQ" || token[subscript].value1 == "LPAREN" || token[subscript].value1 == "RPAREN" || token[subscript].value1 == "SY" || token[subscript].value1 == "COMMA")
		{
			subscript = subscript + 1;
		}
		else
		{
			tmp1->nodekind = "ExpK";
			tmp1->specificnode.stmt = "IdK";
			size2 = size2 + 1;
			printq(size2, outputFile);
			// 这边对赋值语句的不同符号进行区分
			// 加减乘除
			if ((token[subscript].value1 == "PLUS") || (token[subscript].value1 == "MINUS") || (token[subscript].value1 == "TIMES") || (token[subscript].value1 == "OVER"))
			{
				cout << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
				outputFile << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
			}
			else
			{
				// 整数
				if (token[subscript].value1 == "INTC")
				{
					cout << "ExpK" << " " << "const" << " " << token[subscript].value2 << " INTC" << endl;
					outputFile << "ExpK" << " " << "const" << " " << token[subscript].value2 << " INTC" << endl;
				}
				else
				{
					if (token[subscript].value1 == "CHARC")
					{
						cout << "ExpK" << " " << "const" << " " << token[subscript].value2 << " CHARC" << endl;
						outputFile << "ExpK" << " " << "const" << " " << token[subscript].value2 << " CHARC" << endl;
					}
					else
					{
						cout << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
						outputFile << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
					}
				}
			}
			subscript = subscript + 1;
			size2 = size2 - 1;
		}
	}
	if (token[subscript].value1 == "SEMI")
	{
		subscript = subscript + 1;
	}
	return tmp1;
}

// 生成写表达式
Treenode *write1(ofstream &outputFile)
{
	Treenode *tmp1 = new Treenode;
	while ((token[subscript].value1 != "SEMI" && token[subscript].value2 != "END" && token[subscript].value2 != "ENDWH"))
	{
		if (token[subscript].value1 == "LPAREN" || token[subscript].value1 == "RPAREN")
		{
			subscript = subscript + 1;
		}
		else
		{
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
	if (token[subscript].value1 == "SEMI")
	{
		subscript = subscript + 1;
	}
	return tmp1;
}

// 生成读表达式
Treenode *read1(ofstream &outputFile)
{
	Treenode *tmp1 = new Treenode;
	while ((token[subscript].value1 != "SEMI" && token[subscript].value2 != "END" && token[subscript].value2 != "ENDWH"))
	{
		if (token[subscript].value1 == "LPAREN" || token[subscript].value1 == "RPAREN")
		{
			subscript = subscript + 1;
		}
		else
		{
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
	if (token[subscript].value1 == "SEMI")
	{
		subscript = subscript + 1;
	}
	return tmp1;
}

// 生成if表达式
Treenode *if1(ofstream &outputFile)
{
	Treenode *tmp1 = new Treenode;
	while (token[subscript].value2 != "THEN")
	{
		if (token[subscript].value1 == "LPAREN" || token[subscript].value1 == "RPAREN")
		{
			subscript = subscript + 1;
		}
		else
		{
			tmp1->nodekind = "ExpK";
			size2 = size2 + 1;
			printq(size2, outputFile);
			// 这边对赋值语句的不同符号进行区分
			// 加减乘除
			if ((token[subscript].value1 == "PLUS") || (token[subscript].value1 == "MINUS") || (token[subscript].value1 == "TIMES") || (token[subscript].value1 == "OVER") || (token[subscript].value1 == "LT") || (token[subscript].value1 == "RT"))
			{
				cout << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
				outputFile << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
			}
			else
			{
				// 整数
				if (token[subscript].value1 == "INTC")
				{
					cout << "ExpK" << " " << "const" << " " << token[subscript].value2 << " INTC" << endl;
					outputFile << "ExpK" << " " << "const" << " " << token[subscript].value2 << " INTC" << endl;
				}
				else
				{
					cout << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
					outputFile << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
				}
				// cout << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
				// outputFile << "ExpK" << " " << token[subscript].value2 << " " << "IdK" << endl;
			}
			subscript = subscript + 1;
			size2 = size2 - 1;
		}
	}
	return tmp1;
}