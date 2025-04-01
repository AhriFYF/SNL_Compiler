#include "main.h"

// ��������׶�
// �����﷨��
Node* parseSyntaxTree(const string& filePath) {
	ifstream file(filePath);
	if (!file) {
		cerr << "Error: Cannot open file " << filePath << endl;
		return nullptr;
	}

	stack<pair<int, Node*>> nodeStack; // ά���㼶�ͽڵ�
	Node* root = nullptr;
	string line;

	while (getline(file, line)) {
		int indent = 0;
		while (indent < line.size() && (line[indent] == ' ' || line[indent] == '\t')) {
			indent++; // ������������
		}
		line = line.substr(indent); // ȥ�������ո�

		if (line.empty()) continue; // ��������

		istringstream iss(line);
		string type, name, varType, par1, par2;
		iss >> type >> varType >> name >> par1 >> par2; // ��ȡ���͡����������������͡����ܵĲ���1������2

		Node* newNode = nullptr;
		if (name == "param") {
			newNode = new Node(type, par1, par2, "1");
		}
		else {
			newNode = new Node(type, varType, name, "0");
		}

		if (nodeStack.empty()) {
			root = newNode; // ��һ���ڵ���Ϊ���ڵ�
		}
		else {
			// ά����ȷ�Ĳ㼶��ϵ
			while (!nodeStack.empty() && nodeStack.top().first >= indent) {
				nodeStack.pop();
			}
			if (!nodeStack.empty()) {
				nodeStack.top().second->children.push_back(newNode);
			}
		}

		nodeStack.push({ indent, newNode });
	}

	file.close();
	return root;
}

//����﷨��
void printSyntaxTree(Node* node, int depth) {
	if (!node) return;

	// ���ݲ㼶����������
	for (int i = 0; i < depth; ++i) {
		cout << "    "; // ÿ������4���ո�
	}

	// ��ӡ��ǰ�ڵ���Ϣ
	cout << node->type;
	if (!node->name.empty()) cout << " (" << node->name << ")";
	if (!node->varType.empty()) cout << " : " << node->varType;
	cout << endl;

	// �ݹ��ӡ�ӽڵ�
	for (auto child : node->children) {
		printSyntaxTree(child, depth + 1);
	}
}

// ��ӡ��������﷨��
void PrintSyntaxTree(Node* node, int depth, bool isLastChild) {
	if (!node) return;

	// ��ӡ��ǰ�ڵ���Ϣ���������������ߣ�
	for (int i = 0; i < depth - 1; ++i) {
		cout << "    ";
	}
	if (depth > 0) {
		cout << (isLastChild ? "������ " : "������ ");
	}

	cout << node->type;
	if (!node->name.empty()) cout << " (" << node->name << ")";
	if (!node->varType.empty()) cout << " : " << node->varType;
	cout << endl;

	// �ݹ��ӡ�ӽڵ�
	for (size_t i = 0; i < node->children.size(); ++i) {
		PrintSyntaxTree(node->children[i], depth + 1, i == node->children.size() - 1);
	}
}



// ��������׶�

// ���ű�֧��������
// ���캯��
SymbolTable::SymbolTable(SymbolTable* p) : parent(p) {
    // ��ʼ��������ջ
    for (auto& ptr : scopeStack) {
        ptr = nullptr;
    }
}

// ��������
SymbolTable::~SymbolTable() {
    while (currentLevel >= 0) {
        DestroyTable();
    }
}

// ������������
void SymbolTable::EnterScope() {
    currentLevel++;
    currentOffset = 0;
    if (currentLevel > maxlevel) {
        maxlevel = currentLevel;
    }
    scopeStack[currentLevel] = nullptr;
}

// �˳���ǰ������
void SymbolTable::ExitScope() {
    SymbolEntry* p = scopeStack[currentLevel];
    while (p) {
        SymbolEntry* tmp = p;
        p = p->next;
    }
    currentLevel--;
}

// ��ӱ�����ʶ��
bool SymbolTable::AddSymbol(const string& type, SymKind kind, const string& name) {
    if (LookupCurrentScope(name)) {
        cerr << "Error: Duplicate declaration '" << name << "' in level " << currentLevel << endl;
        return false;
    }

    SymbolEntry* entry = new SymbolEntry(name, type, currentLevel, currentOffset++, "");
    entry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = entry;
    return true;
}

// ��ӳ���ͷ
bool SymbolTable::AddSymbolHead(const string& type, SymKind kind, const string& name) {
    if (LookupCurrentScope(name)) {
        cerr << "Error: Duplicate declaration '" << name << "' in level " << currentLevel << endl;
        return false;
    }

    SymbolEntry* entry = new SymbolEntry(name, type, currentLevel, -1, "");
    entry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = entry;
    return true;
}

// ������ͱ�ʶ��
bool SymbolTable::AddSymbolType(const string& type, SymKind kind, const string& name) {
    if (LookupCurrentScope(name)) {
        cerr << "Error: Duplicate declaration '" << name << "' in level " << currentLevel << endl;
        return false;
    }

    SymbolEntry* entry = new SymbolEntry(name, type, currentLevel, -2, "");
    entry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = entry;
    return true;
}

// ��Ӳ���
bool SymbolTable::AddSymbolParam(const string& procedurename, const string& type,
    SymKind kind, const string& name) {
    if (LookupCurrentScope(name)) {
        cerr << "Error: Duplicate declaration '" << name << "' in level " << currentLevel << endl;
        return false;
    }

    SymbolEntry* entry = new SymbolEntry(name, type, currentLevel, currentOffset++, procedurename);
    entry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = entry;
    return true;
}

// ��ӹ��̱�ʶ��
bool SymbolTable::AddSymbolProc(const string& type, SymKind kind, const string& name) {
    if (LookupCurrentScope(name)) {
        cerr << "Error: Duplicate declaration '" << name << "' in level " << currentLevel << endl;
        return false;
    }

    SymbolEntry* entry = new SymbolEntry(name, type, currentLevel, -4, "");
    entry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = entry;
    return true;
}

// �ڵ�ǰ���������
SymbolEntry* SymbolTable::LookupCurrentScope(const string& name) {
    SymbolEntry* p = scopeStack[currentLevel];
    while (p) {
        if (p->name == name) {
            return p;
        }
        p = p->next;
    }
    return nullptr;
}

// �����������
SymbolEntry* SymbolTable::Lookup(const string& name) {
    for (int lv = currentLevel; lv >= 0; lv--) {
        SymbolEntry* p = scopeStack[lv];
        while (p) {
            if (p->name == name) {
                return p;
            }
            p = p->next;
        }
    }
    return nullptr;
}

// �����·��ű�
void SymbolTable::CreateTable() {
    currentLevel++;
    if (currentLevel > maxlevel) {
        maxlevel = currentLevel;
    }
    currentOffset = 0;
    scopeStack[currentLevel] = nullptr;
}

// ���ٵ�ǰ���ű�
void SymbolTable::DestroyTable() {
    SymbolEntry* p = scopeStack[currentLevel];
    while (p) {
        SymbolEntry* tmp = p;
        p = p->next;
        delete tmp;
    }
    currentLevel--;
}

// �ڵ�ǰ�����
bool SymbolTable::SearchOneTable(const string& id, SymbolEntry** entry) {
    SymbolEntry* p = scopeStack[currentLevel];
    while (p) {
        if (p->name == id) {
            if (entry) {
                *entry = p;
            }
            return true;
        }
        p = p->next;
    }
    return false;
}

// ������Ŀ
bool SymbolTable::FindEntry(const string& id, const string& flag, SymbolEntry** entry) {
    if (flag == "one") {
        return SearchOneTable(id, entry);
    }

    if (flag == "up") {
        for (int lv = currentLevel; lv >= 0; lv--) {
            SymbolEntry* p = scopeStack[lv];
            while (p) {
                if (p->name == id) {
                    if (entry) {
                        *entry = p;
                    }
                    return true;
                }
                p = p->next;
            }
        }
    }
    else if (flag == "down") {
        for (int lv = currentLevel; lv < 100 && scopeStack[lv]; lv++) {
            SymbolEntry* p = scopeStack[lv];
            while (p) {
                if (p->name == id) {
                    if (entry) {
                        *entry = p;
                    }
                    return true;
                }
                p = p->next;
            }
        }
    }
    return false;
}

// �ǼǱ�ʶ��
bool SymbolTable::Enter(const string& id, const string& type, SymbolEntry** entry) {
    if (SearchOneTable(id, nullptr)) {
        cerr << "Error: Duplicate declaration of '" << id << "' in scope level "
            << currentLevel << endl;
        return false;
    }

    SymbolEntry* newEntry = new SymbolEntry(id, type, currentLevel, currentOffset, "");
    currentOffset++;

    newEntry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = newEntry;

    if (entry) {
        *entry = newEntry;
    }
    return true;
}

// ��ӡ���ű�
void SymbolTable::PrintSymbolTable(ofstream& outputFile) {
    cout << "===== Symbol Table (Max Level: " << maxlevel << ") =====" << endl;
    outputFile << "===== Symbol Table (Max Level: " << maxlevel << ") =====" << endl;
    for (int lv = maxlevel; lv >= 0; lv--) {
        cout << "--- Level " << lv << " ---" << endl;
        outputFile << "--- Level " << lv << " ---" << endl;
        SymbolEntry* p = scopeStack[lv];
        while (p) {
            if (p->offset == -1) {                      //����ͷ
                cout << "Type: " << p->name << " | Name: " << p->type << endl;
                outputFile << "Type: " << p->name << " | Name: " << p->type << endl;
            }
            else if (p->offset == -2) {                 //����
                cout << "DEFINE     " << "Name: " << p->name << " | Type: " << p->type << endl;
                outputFile << "DEFINE     " << "Name: " << p->name << " | Type: " << p->type << endl;
            }
            else if (p->offset == -4) {                 //����
                cout << "NULL | procKind | Type: " << p->name << " | Name: " << p->type << endl;
                outputFile << "NULL | procKind | Type: " << p->name << " | Name: " << p->type << endl;
            }
            else if (!p->procName.empty()) {            //����
                if (!p->type.empty() && p->type.back() == '^') {
                    cout << "Parameter\tType: " << p->type << " | varKind | indir | Name: " << p->name << " | Offset: " << p->offset << endl;
                    outputFile << "Parameter\tType: " << p->type << " | varKind | indir | Name: " << p->name << " | Offset: " << p->offset << endl;
                }
                else {
                    cout << "Parameter\tType: " << p->type << " | varKind | dir | Name: " << p->name << " | Offset: " << p->offset << endl;
                    outputFile << "Parameter\tType: " << p->type << " | varKind | dir | Name: " << p->name << " | Offset: " << p->offset << endl;
                }
            }
            else {                                      //����
                if (!p->type.empty() && p->type.back() == '^') {
                    cout << "Type: " << p->type << " | varKind | indir | Name: " << p->name << " | Offset: " << p->offset << endl;
                    outputFile << "Type: " << p->type << " | varKind | indir | Name: " << p->name << " | Offset: " << p->offset << endl;
                }
                else {
                    cout << "Type: " << p->type << " | varKind | dir | Name: " << p->name << " | Offset: " << p->offset << endl;
                    outputFile << "Type: " << p->type << " | varKind | dir | Name: " << p->name << " | Offset: " << p->offset << endl;
                }
            }
            p = p->next;
        }
    }
}

// ������
void SymbolTable::AddFieldTable(const string& typeName, FieldChain* fields) {
    fieldTables[typeName] = fields;
}

FieldChain* SymbolTable::GetFieldTable(const string& typeName) {
    auto it = fieldTables.find(typeName);
    return it != fieldTables.end() ? it->second : nullptr;
}

bool SymbolTable::FindField(const string& id, FieldChain* head, FieldEntry** entry) {
    if (!head || !head->head) {
        if (entry) {
            *entry = nullptr;
        }
        return false;
    }

    FieldEntry* p = head->head;
    while (p) {
        if (p->name == id) {
            if (entry) {
                *entry = p;
            }
            return true;
        }
        p = p->next;
    }

    if (entry) {
        *entry = nullptr;
    }
    return false;
}

bool SymbolTable::FindFieldInTable(const string& typeName,
    const string& fieldName,
    FieldEntry** entry) {
    FieldChain* table = GetFieldTable(typeName);
    return FindField(fieldName, table, entry);
}

// �������
bool SymbolTable::insert(const string& name, const string& type, ofstream& outputFile) {
    if (table.find(name) != table.end()) {
        cout << "Error: Variable '" << name << "' redeclared in the same scope." << endl;
        outputFile << "Error: Variable '" << name << "' redeclared in the same scope." << endl;
        return false;
    }
    table[name] = type;
    return true;
}

// ���ҷ���
string SymbolTable::lookup(const string& name) {
    if (table.find(name) != table.end()) {
        return table[name];
    }
    if (parent) {
        return parent->lookup(name);
    }
    return "ERROR: Undeclared variable '" + name + "'";
}

// �������ű�ĺ��ĺ���
void BuildSymbolTable(Node* node, SymbolTable& symTable) {
    if (!node) return;

    // ����ͬ���͵Ľڵ�
    if (node->type == "Prok") {
        // ������ڵ�
        for (auto child : node->children) {
            BuildSymbolTable(child, symTable);
        }
    }
    else if (node->type == "PheadK") {
        // ����ͷ����ѡ�Ǽǳ�������
        if (!node->name.empty()) {
            symTable.AddSymbolHead(node->name, SymKind::PROC, "PROGRAM");
        }
    }
    else if (node->type == "TYPE") {
        // ��������
        for (auto child : node->children) {
            if (child->type == "Deck") {
                symTable.AddSymbolType(child->name, SymKind::TYPE, child->varType);
            }
        }
    }
    else if (node->type == "VAR") {
        // ��������
        for (auto child : node->children) {
            if (child->type == "Deck") {
                // ������������������ "INTEGER v1 v2"��
                //istringstream iss(child->name);
                //string varName;
                //while (iss >> varName) {
                //    symTable.AddSymbol(varName, SymKind::VAR, child->varType);
                //}
                symTable.AddSymbol(child->name, SymKind::VAR, child->varType);
            }
        }
    }
    else if (node->type == "PROCEDURE") {
        node = node->children[0];
        // ��������
        if (!node->name.empty()) {
            symTable.AddSymbolProc(node->name, SymKind::PROC, "PROCEDURE");
        }

        // ������������
        symTable.EnterScope();

        // ��������;ֲ�����
        for (auto child : node->children) {
            if (child->type == "Deck") {
                // ��������
                symTable.AddSymbolParam(node->name, child->name, SymKind::PARAM, child->varType);
            }
            else if (child->type == "VAR") {
                BuildSymbolTable(child, symTable);
            }
        }

        // ���������
        for (auto child : node->children) {
            if (child->type != "Deck" && child->type != "VAR") {
                BuildSymbolTable(child, symTable);
            }
        }

        // �˳�������
        symTable.ExitScope();
    }
    else {
        // �����ڵ㣨���ȣ������ݹ�
        for (auto child : node->children) {
            BuildSymbolTable(child, symTable);
        }
    }
}

// �������ű�
SymbolNode* parseSymbolTable(const string& filePath) {
    ifstream file(filePath);
    if (!file) {
        cerr << "Error: Cannot open file " << filePath << endl;
        return nullptr;
    }

    SymbolNode* root = nullptr;
    string line;

    int indent = 0;
    while (getline(file, line)) {
        if (line.find("Level")!= string::npos) {
            sscanf_s(line.c_str(), "--- Level %d ---", &indent);    // ��ȡ�㼶
            continue; // �����㼶��
        }

        if (line.empty() || line.find("=====")!= string::npos) continue; // �������к�����

        istringstream iss(line);
        string token, type, name;
        string varType, dir, offset;

        if (line.find("PROGRAM") != string::npos) {                             //������
            iss >> token >> token >> token >> token >> name;                    // Type: PROGRAM | Name: p
            SymbolNode* newNode = new SymbolNode("PROGRAM", name, indent);
            if(root == nullptr) root = newNode;
            else if (newNode != nullptr) {
                root->child = newNode;
                newNode->parent = root;
                root = newNode; // ���¸��ڵ�Ϊ����ڵ�
            }
        }
        else if (line.find("PROCEDURE") != string::npos) {                                            //���̴���
            iss >> token >> token >> token >> token >> token >> token >> token >> token >> name;      // NULL | procKind | Type: PROCEDURE | Name: q
            SymbolNode* newNode = new SymbolNode("PROCEDURE", name, indent);
            if(root == nullptr) root = newNode;
            else if (newNode != nullptr) {
                root->child = newNode;
                newNode->parent = root;
                root = newNode; // ���¸��ڵ�Ϊ����ڵ�
            }
        }
        else if (line.find("Parameter") != string::npos) {     //��������       //Parameter	Type: INTEGER | varKind | dir | Name: i | Offset: 0
            iss >> token >> token >> token >> token >> token >> token >> token >> token >> token >> name;
            SymbolNode* newNode = new SymbolNode("PARAMETER", name, indent);
            if(root == nullptr) root = newNode;
            else if (newNode != nullptr) {
                root->child = newNode;
                newNode->parent = root;
                root = newNode; // ���¸��ڵ�Ϊ����ڵ�
            }
        }
        else if (line.find("DEFINE") != string::npos) {         //���ʹ���
            iss >> token >> token >> name >> token >> token;                 //DEFINE     Name: t1 | Type: INTEGER
            SymbolNode* newNode = new SymbolNode("DEFINE", name, indent);
            if(root == nullptr) root = newNode;
            else if (newNode != nullptr) {
                root->child = newNode;
                newNode->parent = root;
                root = newNode; // ���¸��ڵ�Ϊ����ڵ�
            }
        }
        else{                                                   //��������      //Type: INTEGER | varKind | dir | Name: v1 | Offset: 0
            iss >> token >> token >> token >> token >> token >> token >> token >> token >> name >> token >> token;
            SymbolNode* newNode = new SymbolNode("VARIABLE", name, indent);
            if(root == nullptr) root = newNode;
            else if (newNode != nullptr) {
                root->child = newNode;
                newNode->parent = root;
                root = newNode; // ���¸��ڵ�Ϊ����ڵ�
            }
        }
    }

    file.close();
    return root;
}

// ��ӡ������ķ��ű�
void printSymbolTable(SymbolNode* node) {
    if (node == nullptr) return;

    // ��ӡ��ǰ�ڵ���Ϣ
    cout << "Type: " << node->type << " | Name: " << node->name << " | Level: " << node->level << endl;

    // �ݹ��ӡ�ӽڵ�
    printSymbolTable(node->parent);
}


// �����������
void semanticAnalysis(Node* tree, SymbolTable* symTable, ofstream& outputFile, SymbolNode* Parsedsymboltable) {
    if (!tree) return;

    // �������ͺͱ�������
    if (tree->type == "TYPE" || tree->type == "VAR") {
        for (auto child : tree->children) {
            if (!symTable->insert(child->name, child->varType, outputFile)) {
                outputFile << "Error: Variable '" << child->name << "' redeclared in the same scope." << endl;
            }
        }
    }

    // �����������
    if (tree->type == "PROCEDURE") {
        SymbolTable* newScope = new SymbolTable(symTable);
        for (auto child : tree->children) {
            semanticAnalysis(child, newScope, outputFile, Parsedsymboltable);
        }
    }

    // ��������
    if (tree->type == "StmLK") {
        for (auto stmt : tree->children) {
            if (stmt->type == "READ" || stmt->type == "WRITE") {
                string res = symTable->lookup(stmt->name);
                if (res.find("ERROR") != string::npos) {
                    outputFile << res << endl;
                }
            }

            // ��ֵ��䴦��
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

            // ������䴦��
            if (stmt->type == "IF" || stmt->type == "WHILE") {
                string condType = symTable->lookup(stmt->children[0]->name);
                if (condType != "BOOLEAN") {
                    outputFile << "Error: Condition expression must be of BOOLEAN type." << endl;
                }
            }

            // ���̵��ô���
            if (stmt->type == "CALL") {
                string procType = symTable->lookup(stmt->name);
                if (procType != "PROCEDURE") {
                    outputFile << "Error: Identifier '" << stmt->name << "' is not a procedure." << endl;
                }
            }
        }
    }

    // �ݹ�����ӽڵ�
    for (auto child : tree->children) {
        semanticAnalysis(child, symTable, outputFile, Parsedsymboltable);
    }
}