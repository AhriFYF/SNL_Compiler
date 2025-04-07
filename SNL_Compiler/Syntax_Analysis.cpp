#include "main.h"
#define MAX_LINE_LEN 256  // ������󳤶�
#define MAX_FIELD_LEN 128 // �ֶ���󳤶�

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
// �����﷨�������ĸ��ڵ�
Treenode *program(ofstream &outputFile)
{
	Treenode *root = new Treenode;
	if (root)
	{
		cout << "\n"
			 << "���򾭹��﷨������֮����﷨������Ϊ:" << endl;
		cout << "Prok" << endl;

		outputFile << "Prok" << endl;
		root->idchild = 3;
		root->child[0] = programhead(outputFile); // ��������ͷͷ�����������ڵ�
		root->child[1] = declarehead(outputFile); // �����������������ڵ�
		root->child[2] = probody(outputFile);	  // ���������еĺ�����
		root->sibling = NULL;
		root->nodekind = "Prok"; // �ڵ�����Ϊ���ڵ�
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

// ��������ͷͷ�����������ڵ�
Treenode *programhead(ofstream &outputFile)
{
	Treenode *tmp;
	tmp = new Treenode;
	if (tmp)
	{
		if (token[subscript].value2 == "PROGRAM")
		{
			size2 = size2 + 1;
			printq(size2, outputFile); // ���ô�ӡ�ո�
			cout << "PheadK" << " ";
			outputFile << "PheadK" << " ";
			tmp->idchild = 0;
			tmp->nodekind = "PheadK";
			// tmp->sibling = declarehead(outputFile);
			subscript = subscript + 1;
			cout << token[subscript].value2 << endl;
			outputFile << token[subscript].value2 << endl;
			size2 = size2 - 1; // ����
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

// �����������������ڵ�
Treenode *declarehead(ofstream &outputFile)
{
	Treenode *tmp1 = new Treenode;
	Treenode *tmp2 = new Treenode;
	Treenode *tmp3 = new Treenode;
	;
	Treenode *tp1 = typedec(outputFile);
	Treenode *tp2 = vardec(outputFile);
	Treenode *tp3 = procdec(outputFile);
	// �Թ��������ڵ��ϸ��
	if (tmp3)
	{
		tmp3->child[0] = tp3;
		tmp3->sibling = NULL;
		tmp3->nodekind = "Proc";
	}
	// �Ա��������ڵ��ϸ��
	if (tmp2)
	{
		tmp2->child[0] = tp2;
		tmp2->sibling = tmp3;
		tmp2->nodekind = "Varc";
	}
	// �����������ڵ��ϸ��
	if (tmp1)
	{
		tmp1->child[0] = tp1;
		tmp1->nodekind = "TypeK";
		tmp1->sibling = tmp2;
	}
	// cout << 3;
	// �жϲ�ͬ����õ��ķ���ֵ
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
			return tmp3; // �����ǲ���NULL��������ô����
		}
	}
	return NULL;
}
//
////���������庯���ڵ�
// Treenode* programbody() {
//	return NULL;
// }

// �����������ִ������
Treenode *typedec(ofstream &outputFile)
{
	Treenode *t = new Treenode;
	if (token[subscript].value2 == "TYPE")
	{
		size2 = size2 + 1;
		printq(size2, outputFile); // ���ô�ӡ�ո�
		cout << "TYPE" << endl;
		outputFile << "TYPE" << endl;
		subscript = subscript + 1;
		// �������������ڵ�
		t->nodekind = "Typek";
		t->child[0] = typedeclist(outputFile);
		size2 = size2 - 1; // ע����˵�ʱ��
		return t;
	}
	else
		return NULL;
}

// ���������������
Treenode *vardec(ofstream &outputFile)
{
	Treenode *t = new Treenode;
	if (token[subscript].value2 == "VAR")
	{
		size2 = size2 + 1;
		printq(size2, outputFile); // ���ô�ӡ�ո�
		cout << "VAR" << endl;
		outputFile << "VAR" << endl;
		subscript = subscript + 1;
		// �������������ڵ�
		t->nodekind = "Vark";
		t->child[0] = vardeclist(outputFile);
		size2 = size2 - 1; // ע����˵�ʱ��
		return t;
	}
	else
		return NULL;
}

// ���������������
Treenode *procdec(ofstream &outputFile)
{
	Treenode *t = new Treenode;
	if (token[subscript].value2 == "PROCEDURE")
	{
		size2 = size2 + 1;
		printq(size2, outputFile); // ���ô�ӡ�ո�
		cout << "PROCEDURE" << endl;
		outputFile << "PROCEDURE" << endl;
		subscript = subscript + 1;
		// �������������ڵ�
		t->nodekind = "PROCEDURE";
		t->child[0] = paramdeclist(outputFile);
		size2 = size2 - 1; // ע����˵�ʱ��
		return t;
	}
	else
		return NULL;
}

// ���������ľ��庯��
Treenode *typedeclist(ofstream &outputFile)
{
	if (token[subscript].value2 == "VAR" || token[subscript].value2 == "PROCEDURE" ||
		token[subscript].value2 == "BEGIN" || token[subscript].value1 == "DOT")
	{ // ����Ҫϸ��һ���סvalue1��value2
		return NULL;
	}
	else
	{
		Treenode *t = new Treenode;
		if (t)
		{
			// ������Ϣ
			t->value = token[subscript].value2;
			subscript = subscript + 2;
			t->nodekind = "Deck";
			t->specificnode.dec = token[subscript].value2;
			subscript = subscript + 2;
			// ��ӡ
			size2 = size2 + 1;
			printq(size2, outputFile);
			cout << t->nodekind << " " << t->specificnode.dec << " " << t->value << endl;
			outputFile << t->nodekind << " " << t->specificnode.dec << " " << t->value << endl;
			size2 = size2 - 1; // ע�����ʱ��
			// �ݹ����
			t->sibling = typedeclist(outputFile);
		}
		return t;
	}
}

// ���������ľ��庯��
Treenode *vardeclist(ofstream &outputFile)
{
	if (token[subscript].value2 == "PROCEDURE" || token[subscript].value2 == "BEGIN" || token[subscript].value1 == "DOT")
	{ // ����Ҫϸ��һ���סvalue1��value2
		return NULL;
	}
	else
	{
		if (token[subscript].value2 == "VAR")
			subscript = subscript + 1;
		Treenode *t = new Treenode;
		if (t)
		{
			// ��������ڵ�ľ�����
			t->nodekind = "Deck";
			t->specificnode.dec = token[subscript].value2;
			subscript = subscript + 1;
			while (token[subscript].value1 != "SEMI")
			{
				// �����token��������,������;ʱ
				t->id[t->idnum] = token[subscript].value2;
				t->idnum = t->idnum + 1;
				subscript = subscript + 1;
				// ������������������ж�
				if (token[subscript].value1 == "JSP1")
				{
					subscript = subscript + 1;
				}
			}
			// ��;����һ��token
			subscript = subscript + 1;
			// ��ӡ����
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
			// �ݹ����
			t->sibling = vardeclist(outputFile);
		}
	}
}

// ���������еĲ��ֺ���
Treenode *paramdeclist(ofstream &outputFile)
{
	if (token[subscript].value1 == "ID" || token[subscript].value1 == "CHAR")
	{
		// ���ɺ������ڵ㲿����Ϣ
		Treenode *t = new Treenode;
		t->nodekind = "HDeck"; // �Դ���һ���������ڵ�
		t->value = token[subscript].value2;
		subscript = subscript + 1;
		// ��ӡ��Ϣ
		size2 = size2 + 1;
		printq(size2, outputFile);
		cout << "HDeck" << " " << t->value << endl;
		outputFile << "HDeck" << " " << t->value << endl;
		// �����βΣ����������������岿�֣���Ϊ���ӽڵ�
		t->child[0] = paramlist(outputFile); // ���������е��βκ���
		t->child[1] = vardec(outputFile);	 // ���������еı�������
		t->child[2] = probody(outputFile);	 // ���������еĺ�����
		size2 = size2 - 1;
		return t;
	}
	else
		return NULL;
}

// ���������е��βκ���
Treenode *paramlist(ofstream &outputFile)
{
	// ��ȥ֮��������һ��������
	subscript = subscript + 1;
	// ���кö���β�ʱ,���ɽڵ�
	Treenode *t = new Treenode;
	while (token[subscript].value1 != "RPAREN")
	{
		if (token[subscript].value1 == "JSP1")
		{
			// ����,������
			subscript = subscript + 1;
		}
		t->id[t->idnum] = token[subscript].value2;
		t->idnum = t->idnum + 1;
		subscript = subscript + 1;
	}
	// ����token����������
	subscript = subscript + 2;
	// ��ӡ��Ϣ
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

// ���������еĺ�����
Treenode *probody(ofstream &outputFile)
{
	Treenode *t = new Treenode;
	if (token[subscript].value2 == "BEGIN")
	{
		size2 = size2 + 1;
		printq(size2, outputFile); // ���ô�ӡ�ո�
		cout << "StmLK" << endl;
		outputFile << "StmLK" << endl;
		subscript = subscript + 1;
		// ���ɺ��������������ڵ�
		t->nodekind = "StmLK";
		// ��߽����жϣ��ǲ�����䶼������
		while (token[subscript].value2 != "END" && token[subscript].value2 != "ENDWH")
		{
			t->child[t->idchild] = stmt(outputFile);
			t->idchild = t->idchild + 1;
		}
		subscript = subscript + 1;
		size2 = size2 - 1; // ע����˵�ʱ��
		return t;
	}
	else
		return NULL;
}

// �õ�һ�����ڵ�
Treenode *stmt(ofstream &outputFile)
{
	if (token[subscript].value1 == "ID" || token[subscript].value1 == "CHARC" || token[subscript].value1 == "INTC")
	{
		// ˵���Ǹ�ֵ�ڵ����,���ɸ�ֵ�ڵ�
		Treenode *tmp1 = new Treenode;
		tmp1->nodekind = "StmtK";
		tmp1->specificnode.stmt = "AssignK";
		// ��ӡ���
		size2 = size2 + 1;
		printq(size2, outputFile);
		// ��߼�һЩ�ж������ֺ������ú͸�ֵ���
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
		// �������ɸ�ֵ�ڵ�ϸ�Ĳ��֣�
		tmp1->child[0] = assign1(outputFile);
		size2 = size2 - 1;
		return tmp1;
	}
	else
	{
		if (token[subscript].value2 == "WRITE")
		{
			// ˵����д���ڵ�
			Treenode *tmp1 = new Treenode;
			tmp1->nodekind = "StmtK";
			tmp1->specificnode.stmt = "WRITE";
			subscript = subscript + 1;
			// ��ӡ���
			size2 = size2 + 1;
			printq(size2, outputFile);
			cout << "StmtK" << " " << "WRITE" << endl;
			outputFile << "StmtK" << " " << "WRITE" << endl;
			// ��������д�ڵ�ϸ�Ĳ��֣�
			tmp1->child[0] = write1(outputFile);
			size2 = size2 - 1;
			return tmp1;
		}
		else
		{
			if (token[subscript].value2 == "READ")
			{
				// ˵���Ƕ���ڵ�
				Treenode *tmp1 = new Treenode;
				tmp1->nodekind = "StmtK";
				tmp1->specificnode.stmt = "READ";
				subscript = subscript + 1;
				// ��ӡ���
				size2 = size2 + 1;
				printq(size2, outputFile);
				cout << "StmtK" << " " << "READ" << endl;
				outputFile << "StmtK" << " " << "READ" << endl;
				// �������ɶ��ڵ�ϸ�Ĳ��֣�
				tmp1->child[0] = read1(outputFile);
				size2 = size2 - 1;
				return tmp1;
			}
			else
			{
				if (token[subscript].value2 == "IF")
				{
					// ˵�����жϽڵ�
					Treenode *tmp1 = new Treenode;
					tmp1->nodekind = "StmtK";
					tmp1->specificnode.stmt = "IF";
					subscript = subscript + 1;
					size2 = size2 + 1;
					printq(size2, outputFile);
					cout << "StmtK" << " " << "IF" << endl;
					outputFile << "StmtK" << " " << "IF" << endl;
					// ��������ѡ��ڵ�if�жϲ��֣�
					tmp1->child[0] = if1(outputFile);
					// ��ӡthen����
					size2 = size2 + 1;
					printq(size2, outputFile);
					cout << "StmtK" << " " << "THEN" << endl;
					outputFile << "StmtK" << " " << "THEN" << endl;
					subscript = subscript + 1;
					tmp1->child[2] = assign1(outputFile);
					size2 = size2 - 1;
					// �����else
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
					// ����FI�Ĵ���
					subscript = subscript + 2;
					// ������һ��ʼ��if�ж�
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

// ���ɸ�ֵ���ʽ�ڵ�
Treenode *assign1(ofstream &outputFile)
{
	// ��ļ�һ�㣬������;��ֹͣ
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
			// ��߶Ը�ֵ���Ĳ�ͬ���Ž�������
			// �Ӽ��˳�
			if ((token[subscript].value1 == "PLUS") || (token[subscript].value1 == "MINUS") || (token[subscript].value1 == "TIMES") || (token[subscript].value1 == "OVER"))
			{
				cout << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
				outputFile << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
			}
			else
			{
				// ����
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

// ����д���ʽ
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

// ���ɶ����ʽ
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

// ����if���ʽ
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
			// ��߶Ը�ֵ���Ĳ�ͬ���Ž�������
			// �Ӽ��˳�
			if ((token[subscript].value1 == "PLUS") || (token[subscript].value1 == "MINUS") || (token[subscript].value1 == "TIMES") || (token[subscript].value1 == "OVER") || (token[subscript].value1 == "LT") || (token[subscript].value1 == "RT"))
			{
				cout << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
				outputFile << "ExpK" << " " << "OP" << " " << token[subscript].value2 << endl;
			}
			else
			{
				// ����
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