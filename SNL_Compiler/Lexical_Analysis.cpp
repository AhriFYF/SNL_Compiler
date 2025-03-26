#include "main.h"

//打印出错函数
void printwrong() {
	cout << "输入的程序段有词法错误" << endl;
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