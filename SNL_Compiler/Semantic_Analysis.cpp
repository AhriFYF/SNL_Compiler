#include "main.h"

// 语义分析阶段
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
		string type, name, varType, par1, par2;
		iss >> type >> varType >> name >> par1 >> par2; // 读取类型、变量名、变量类型、可能的参数1、参数2

		Node* newNode = nullptr;
		if (name == "param") {
			newNode = new Node(type, par1, par2, "1");
		}
		else {
			newNode = new Node(type, varType, name, "0");
		}

		if (nodeStack.empty()) {
			root = newNode; // 第一个节点作为根节点
		}
		else {
			// 维护正确的层级关系
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

//检查语法树
void printSyntaxTree(Node* node, int depth) {
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

// 打印解析后的语法树
void PrintSyntaxTree(Node* node, int depth, bool isLastChild) {
	if (!node) return;

	// 打印当前节点信息（带缩进和连接线）
	for (int i = 0; i < depth - 1; ++i) {
		cout << "    ";
	}
	if (depth > 0) {
		cout << (isLastChild ? "└── " : "├── ");
	}

	cout << node->type;
	if (!node->name.empty()) cout << " (" << node->name << ")";
	if (!node->varType.empty()) cout << " : " << node->varType;
	cout << endl;

	// 递归打印子节点
	for (size_t i = 0; i < node->children.size(); ++i) {
		PrintSyntaxTree(node->children[i], depth + 1, i == node->children.size() - 1);
	}
}



// 语义分析阶段

// 符号表（支持作用域）
// 构造函数
SymbolTable::SymbolTable(SymbolTable* p) : parent(p) {
    // 初始化作用域栈
    for (auto& ptr : scopeStack) {
        ptr = nullptr;
    }
}

// 析构函数
SymbolTable::~SymbolTable() {
    while (currentLevel >= 0) {
        DestroyTable();
    }
}

// 进入新作用域
void SymbolTable::EnterScope() {
    currentLevel++;
    currentOffset = 0;
    if (currentLevel > maxlevel) {
        maxlevel = currentLevel;
    }
    scopeStack[currentLevel] = nullptr;
}

// 退出当前作用域
void SymbolTable::ExitScope() {
    SymbolEntry* p = scopeStack[currentLevel];
    while (p) {
        SymbolEntry* tmp = p;
        p = p->next;
    }
    currentLevel--;
}

// 添加变量标识符
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

// 添加程序头
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

// 添加类型标识符
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

// 添加参数
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

// 添加过程标识符
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

// 在当前作用域查找
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

// 跨作用域查找
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

// 创建新符号表
void SymbolTable::CreateTable() {
    currentLevel++;
    if (currentLevel > maxlevel) {
        maxlevel = currentLevel;
    }
    currentOffset = 0;
    scopeStack[currentLevel] = nullptr;
}

// 销毁当前符号表
void SymbolTable::DestroyTable() {
    SymbolEntry* p = scopeStack[currentLevel];
    while (p) {
        SymbolEntry* tmp = p;
        p = p->next;
        delete tmp;
    }
    currentLevel--;
}

// 在当前表查找
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

// 查找条目
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

// 登记标识符
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

// 打印符号表
void SymbolTable::PrintSymbolTable(ofstream& outputFile) {
    cout << "===== Symbol Table (Max Level: " << maxlevel << ") =====" << endl;
    outputFile << "===== Symbol Table (Max Level: " << maxlevel << ") =====" << endl;
    for (int lv = maxlevel; lv >= 0; lv--) {
        cout << "--- Level " << lv << " ---" << endl;
        outputFile << "--- Level " << lv << " ---" << endl;
        SymbolEntry* p = scopeStack[lv];
        while (p) {
            if (p->offset == -1) {                      //程序头
                cout << "Type: " << p->name << " | Name: " << p->type << endl;
                outputFile << "Type: " << p->name << " | Name: " << p->type << endl;
            }
            else if (p->offset == -2) {                 //类型
                cout << "DEFINE     " << "Name: " << p->name << " | Type: " << p->type << endl;
                outputFile << "DEFINE     " << "Name: " << p->name << " | Type: " << p->type << endl;
            }
            else if (p->offset == -4) {                 //过程
                cout << "NULL | procKind | Type: " << p->name << " | Name: " << p->type << endl;
                outputFile << "NULL | procKind | Type: " << p->name << " | Name: " << p->type << endl;
            }
            else if (!p->procName.empty()) {            //参数
                if (!p->type.empty() && p->type.back() == '^') {
                    cout << "Parameter\tType: " << p->type << " | varKind | indir | Name: " << p->name << " | Offset: " << p->offset << endl;
                    outputFile << "Parameter\tType: " << p->type << " | varKind | indir | Name: " << p->name << " | Offset: " << p->offset << endl;
                }
                else {
                    cout << "Parameter\tType: " << p->type << " | varKind | dir | Name: " << p->name << " | Offset: " << p->offset << endl;
                    outputFile << "Parameter\tType: " << p->type << " | varKind | dir | Name: " << p->name << " | Offset: " << p->offset << endl;
                }
            }
            else {                                      //变量
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

// 域表操作
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

// 插入符号
bool SymbolTable::insert(const string& name, const string& type, ofstream& outputFile) {
    if (table.find(name) != table.end()) {
        cout << "Error: Variable '" << name << "' redeclared in the same scope." << endl;
        outputFile << "Error: Variable '" << name << "' redeclared in the same scope." << endl;
        return false;
    }
    table[name] = type;
    return true;
}

// 查找符号
string SymbolTable::lookup(const string& name) {
    if (table.find(name) != table.end()) {
        return table[name];
    }
    if (parent) {
        return parent->lookup(name);
    }
    return "ERROR: Undeclared variable '" + name + "'";
}

// 构建符号表的核心函数
void BuildSymbolTable(Node* node, SymbolTable& symTable) {
    if (!node) return;

    // 处理不同类型的节点
    if (node->type == "Prok") {
        // 程序根节点
        for (auto child : node->children) {
            BuildSymbolTable(child, symTable);
        }
    }
    else if (node->type == "PheadK") {
        // 程序头（可选登记程序名）
        if (!node->name.empty()) {
            symTable.AddSymbolHead(node->name, SymKind::PROC, "PROGRAM");
        }
    }
    else if (node->type == "TYPE") {
        // 类型声明
        for (auto child : node->children) {
            if (child->type == "Deck") {
                symTable.AddSymbolType(child->name, SymKind::TYPE, child->varType);
            }
        }
    }
    else if (node->type == "VAR") {
        // 变量声明
        for (auto child : node->children) {
            if (child->type == "Deck") {
                // 处理多个变量声明（如 "INTEGER v1 v2"）
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
        // 过程声明
        if (!node->name.empty()) {
            symTable.AddSymbolProc(node->name, SymKind::PROC, "PROCEDURE");
        }

        // 进入新作用域
        symTable.EnterScope();

        // 处理参数和局部变量
        for (auto child : node->children) {
            if (child->type == "Deck") {
                // 参数声明
                symTable.AddSymbolParam(node->name, child->name, SymKind::PARAM, child->varType);
            }
            else if (child->type == "VAR") {
                BuildSymbolTable(child, symTable);
            }
        }

        // 处理过程体
        for (auto child : node->children) {
            if (child->type != "Deck" && child->type != "VAR") {
                BuildSymbolTable(child, symTable);
            }
        }

        // 退出作用域
        symTable.ExitScope();
    }
    else {
        // 其他节点（语句等）继续递归
        for (auto child : node->children) {
            BuildSymbolTable(child, symTable);
        }
    }
}

// 解析符号表
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
            sscanf_s(line.c_str(), "--- Level %d ---", &indent);    // 读取层级
            continue; // 跳过层级行
        }

        if (line.empty() || line.find("=====")!= string::npos) continue; // 跳过空行和首行

        istringstream iss(line);
        string token, type, name;
        string varType, dir, offset;

        if (line.find("PROGRAM") != string::npos) {                             //程序处理
            iss >> token >> token >> token >> token >> name;                    // Type: PROGRAM | Name: p
            SymbolNode* newNode = new SymbolNode("PROGRAM", name, indent);
            if(root == nullptr) root = newNode;
            else if (newNode != nullptr) {
                root->child = newNode;
                newNode->parent = root;
                root = newNode; // 更新根节点为程序节点
            }
        }
        else if (line.find("PROCEDURE") != string::npos) {                                            //过程处理
            iss >> token >> token >> token >> token >> token >> token >> token >> token >> name;      // NULL | procKind | Type: PROCEDURE | Name: q
            SymbolNode* newNode = new SymbolNode("PROCEDURE", name, indent);
            if(root == nullptr) root = newNode;
            else if (newNode != nullptr) {
                root->child = newNode;
                newNode->parent = root;
                root = newNode; // 更新根节点为程序节点
            }
        }
        else if (line.find("Parameter") != string::npos) {     //参数处理       //Parameter	Type: INTEGER | varKind | dir | Name: i | Offset: 0
            iss >> token >> token >> token >> token >> token >> token >> token >> token >> token >> name;
            SymbolNode* newNode = new SymbolNode("PARAMETER", name, indent);
            if(root == nullptr) root = newNode;
            else if (newNode != nullptr) {
                root->child = newNode;
                newNode->parent = root;
                root = newNode; // 更新根节点为程序节点
            }
        }
        else if (line.find("DEFINE") != string::npos) {         //类型处理
            iss >> token >> token >> name >> token >> token;                 //DEFINE     Name: t1 | Type: INTEGER
            SymbolNode* newNode = new SymbolNode("DEFINE", name, indent);
            if(root == nullptr) root = newNode;
            else if (newNode != nullptr) {
                root->child = newNode;
                newNode->parent = root;
                root = newNode; // 更新根节点为程序节点
            }
        }
        else{                                                   //变量处理      //Type: INTEGER | varKind | dir | Name: v1 | Offset: 0
            iss >> token >> token >> token >> token >> token >> token >> token >> token >> name >> token >> token;
            SymbolNode* newNode = new SymbolNode("VARIABLE", name, indent);
            if(root == nullptr) root = newNode;
            else if (newNode != nullptr) {
                root->child = newNode;
                newNode->parent = root;
                root = newNode; // 更新根节点为程序节点
            }
        }
    }

    file.close();
    return root;
}

// 打印解析后的符号表
void printSymbolTable(SymbolNode* node) {
    if (node == nullptr) return;

    // 打印当前节点信息
    cout << "Type: " << node->type << " | Name: " << node->name << " | Level: " << node->level << endl;

    // 递归打印子节点
    printSymbolTable(node->parent);
}


// 语义分析函数
void semanticAnalysis(Node* tree, SymbolTable* symTable, ofstream& outputFile, SymbolNode* Parsedsymboltable) {
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
            semanticAnalysis(child, newScope, outputFile, Parsedsymboltable);
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
        semanticAnalysis(child, symTable, outputFile, Parsedsymboltable);
    }
}