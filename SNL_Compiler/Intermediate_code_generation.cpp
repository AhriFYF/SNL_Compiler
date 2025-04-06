#include "main.h"

int tempCount = 0;
int labelCount = 0;
vector<Quadruple> quads;

string newTemp() {
    return "t" + to_string(tempCount++);
}

string newLabel() {
    return "L" + to_string(labelCount++);
}

void emit(const string& op, const string& arg1, const string& arg2, const string& result) {
    quads.push_back({op, arg1, arg2, result});
}

void writeToFile(const string& filename) {
    ofstream file(filename);
    for (auto& q : quads) {
        cout << "(" << setw(4) << q.op << ", " << setw(4) << q.arg1 << ", " << setw(4) << q.arg2 << ", " << setw(4) << q.result << ")\n";
        file << "(" << setw(4) << q.op << ", " << setw(4) << q.arg1 << ", " << setw(4) << q.arg2 << ", " << setw(4) << q.result << ")\n";
    }
    file.close();
}

// 从语法树文本中生成中间代码
void processSyntaxTree(istream& input) {
    string line;
    while (getline(input, line)) {
        int indent = line.find_first_not_of(" \t");
        string node = line.substr(indent);

        if (node.find("StmtK READ") != string::npos) {
            getline(input, line);
            string var = line.substr(line.find_last_of(' ') + 1);
            emit("read", "-", "-", var);
        }
        else if (node.find("StmtK WRITE") != string::npos) {
            getline(input, line);
            string var = line.substr(line.find_last_of(' ') + 1);
            emit("write", "-", "-", var);
        }
        else if (node.find("StmtK AssignK") != string::npos) {
            getline(input, line); // left
            string left = line.substr(line.find_last_of(' ') + 1);
            getline(input, line); // right
            string right = line.substr(line.find_last_of(' ') + 1);
            emit("=", right, "-", left);
        }
        else if (node.find("StmtK IF") != string::npos) {
            string var1, op, constVal;
            getline(input, line); var1 = line.substr(line.find_last_of(' ') + 1); // v1
            getline(input, line); op = line.substr(line.find_last_of(' ') + 1);   // <
            getline(input, line); constVal = line.substr(line.find_last_of(' ') + 1); // 10

            string labelTrue = newLabel();
            string labelFalse = newLabel();
            string labelEnd = newLabel();

            emit(op, var1, constVal, labelTrue);
            emit("goto", "-", "-", labelFalse);

            // THEN
            getline(input, line); // StmtK THEN
            getline(input, line); string t1_left = line.substr(line.find_last_of(' ') + 1);
            getline(input, line); string t1_op1 = line.substr(line.find_last_of(' ') + 1);
            getline(input, line); string t1_op = line.substr(line.find_last_of(' ') + 1);
            getline(input, line); string t1_op2 = line.substr(line.find_last_of(' ') + 1);

            string temp1 = newTemp();
            emit(t1_op, t1_op1, t1_op2, temp1);
            emit("=", temp1, "-", t1_left);
            emit("goto", "-", "-", labelEnd);

            // ELSE
            emit("label", "-", "-", labelFalse);
            getline(input, line); // StmtK ELSE
            getline(input, line); string t2_left = line.substr(line.find_last_of(' ') + 1);
            getline(input, line); string t2_op1 = line.substr(line.find_last_of(' ') + 1);
            getline(input, line); string t2_op = line.substr(line.find_last_of(' ') + 1);
            getline(input, line); string t2_op2 = line.substr(line.find_last_of(' ') + 1);

            string temp2 = newTemp();
            emit(t2_op, t2_op1, t2_op2, temp2);
            emit("=", temp2, "-", t2_left);
            emit("label", "-", "-", labelEnd);
        }
        else if (node.find("StmtK CALL") != string::npos) {
            getline(input, line); string proc = line.substr(line.find_last_of(' ') + 1); // q
            getline(input, line); string param = line.substr(line.find_last_of(' ') + 1); // v1
            emit("call", proc, param, "-");
        }
        else if (node.find("=,") != string::npos || node.find("call") != string::npos) {
            // Already handled
        }
    }
}