#include "main.h"

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
bool SymbolTable::AddSymbol(const std::string& type, SymKind kind, const std::string& name) {
    if (LookupCurrentScope(name)) {
        std::cerr << "Error: Duplicate declaration '" << name << "' in level " << currentLevel << std::endl;
        return false;
    }

    SymbolEntry* entry = new SymbolEntry(name, type, currentLevel, currentOffset++, "");
    entry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = entry;
    return true;
}

// 添加程序头
bool SymbolTable::AddSymbolHead(const std::string& type, SymKind kind, const std::string& name) {
    if (LookupCurrentScope(name)) {
        std::cerr << "Error: Duplicate declaration '" << name << "' in level " << currentLevel << std::endl;
        return false;
    }

    SymbolEntry* entry = new SymbolEntry(name, type, currentLevel, -1, "");
    entry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = entry;
    return true;
}

// 添加类型标识符
bool SymbolTable::AddSymbolType(const std::string& type, SymKind kind, const std::string& name) {
    if (LookupCurrentScope(name)) {
        std::cerr << "Error: Duplicate declaration '" << name << "' in level " << currentLevel << std::endl;
        return false;
    }

    SymbolEntry* entry = new SymbolEntry(name, type, currentLevel, -2, "");
    entry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = entry;
    return true;
}

// 添加参数
bool SymbolTable::AddSymbolParam(const std::string& procedurename, const std::string& type,
    SymKind kind, const std::string& name) {
    if (LookupCurrentScope(name)) {
        std::cerr << "Error: Duplicate declaration '" << name << "' in level " << currentLevel << std::endl;
        return false;
    }

    SymbolEntry* entry = new SymbolEntry(name, type, currentLevel, currentOffset++, procedurename);
    entry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = entry;
    return true;
}

// 添加过程标识符
bool SymbolTable::AddSymbolProc(const std::string& type, SymKind kind, const std::string& name) {
    if (LookupCurrentScope(name)) {
        std::cerr << "Error: Duplicate declaration '" << name << "' in level " << currentLevel << std::endl;
        return false;
    }

    SymbolEntry* entry = new SymbolEntry(name, type, currentLevel, -4, "");
    entry->next = scopeStack[currentLevel];
    scopeStack[currentLevel] = entry;
    return true;
}

// 在当前作用域查找
SymbolEntry* SymbolTable::LookupCurrentScope(const std::string& name) {
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
SymbolEntry* SymbolTable::Lookup(const std::string& name) {
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
bool SymbolTable::SearchOneTable(const std::string& id, SymbolEntry** entry) {
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
bool SymbolTable::FindEntry(const std::string& id, const std::string& flag, SymbolEntry** entry) {
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
bool SymbolTable::Enter(const std::string& id, const std::string& type, SymbolEntry** entry) {
    if (SearchOneTable(id, nullptr)) {
        std::cerr << "Error: Duplicate declaration of '" << id << "' in scope level "
            << currentLevel << std::endl;
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
void SymbolTable::PrintSymbolTable() {
    std::cout << "===== Symbol Table (Max Level: " << maxlevel << ") =====" << std::endl;
    for (int lv = maxlevel; lv >= 0; lv--) {
        std::cout << "--- Level " << lv << " ---" << std::endl;
        SymbolEntry* p = scopeStack[lv];
        while (p) {
            if (p->offset == -1) {
                std::cout << "Type: " << p->name << " | Name: " << p->type << std::endl;
            }
            else if (p->offset == -2) {
                std::cout << "Name: " << p->name << " | Type: " << p->type << std::endl;
            }
            else if (p->offset == -4) {
                std::cout << "NULL | procKind | Type: " << p->name << " | Name: " << p->type << std::endl;
            }
            else if (!p->procName.empty()) {
                if (!p->type.empty() && p->type.back() == '^') {
                    std::cout << "Parameter\tType: " << p->type << " | varKind | indir | Name: " << p->name << " | Offset: " << p->offset << std::endl;
                }
                else {
                    std::cout << "Parameter\tType: " << p->type << " | varKind | dir | Name: " << p->name << " | Offset: " << p->offset << std::endl;
                }
            }
            else {
                if (!p->type.empty() && p->type.back() == '^') {
                    std::cout << "Type: " << p->type << " | varKind | indir | Name: " << p->name << " | Offset: " << p->offset << std::endl;
                }
                else {
                    std::cout << "Type: " << p->type << " | varKind | dir | Name: " << p->name << " | Offset: " << p->offset << std::endl;
                }
            }
            p = p->next;
        }
    }
}

// 域表操作
void SymbolTable::AddFieldTable(const std::string& typeName, FieldChain* fields) {
    fieldTables[typeName] = fields;
}

FieldChain* SymbolTable::GetFieldTable(const std::string& typeName) {
    auto it = fieldTables.find(typeName);
    return it != fieldTables.end() ? it->second : nullptr;
}

bool SymbolTable::FindField(const std::string& id, FieldChain* head, FieldEntry** entry) {
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

bool SymbolTable::FindFieldInTable(const std::string& typeName,
    const std::string& fieldName,
    FieldEntry** entry) {
    FieldChain* table = GetFieldTable(typeName);
    return FindField(fieldName, table, entry);
}

// 插入符号
bool SymbolTable::insert(const std::string& name, const std::string& type, std::ofstream& outputFile) {
    if (table.find(name) != table.end()) {
        std::cout << "Error: Variable '" << name << "' redeclared in the same scope." << std::endl;
        outputFile << "Error: Variable '" << name << "' redeclared in the same scope." << std::endl;
        return false;
    }
    table[name] = type;
    return true;
}

// 查找符号
std::string SymbolTable::lookup(const std::string& name) {
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

// 语义分析阶段
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