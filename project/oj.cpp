
#include <iostream>
#include <stdio.h>

#include<fstream>
#include<cstring>
#include<string>
#include<bits/stdc++.h>
#include <vector>
#include <cctype>  // 用于 isdigit 函数

using namespace std;

bool ifcout = 0;
struct InstructionNum {
    int num;
    bool ifhave;
} instructionNum;
int level;
bool ifnormalend = 1; // 判断是否正常结束
int robotcurrentx = 11; // 机器人现在的x坐标（纵坐标永远是19）
int instructionnum;
int outboxshuzu[30];
int outboxshuzuxiabiao = 1;
int geichuinboxshuzuxiabiao = 0; // 当前输入传送带第一个元素所在的下标，从0开始

// 指令打包成结构体
struct Instruction {
    string name;
    int num;
    int ifused; // 帮助储存一共进行了多少次指令
};
Instruction instruction[105];

// 机器人手头的东西
struct Currentblock {
    bool ifhave;
    int num;
} currentblock;

// 空地的数字
struct Blankspace {
    bool ifhave;
    int num;
} blankspace[3]; // 空地下标从0开始

// 关卡信息打包成结构体
struct Levelinfo {
    int inboxnum;
    int geichuinboxshuzu[10];
    int outboxnum;
    int yuqishuchu[10]; // 存储正确的输出
    int availileblanknum;
    int abailableinstructionnum;
    vector<string> availableinstruction;
    string levelMission;
};

Levelinfo levelinfo[3] = {
    {2, {1, 2}, 2, {1, 2}, 0, 2, {"inbox", "outbox"}, "Let the robot take each block in the input sequence and put it into the output sequence"},
    {8, {3, 9, 5, 1, -2, -2, 9, -9}, 8, {-6, 6, 4, -4, 0, 0, 18, -18}, 3, 8, {"inbox", "outbox", "copyfrom", "copyto", "add", "sub", "jump", "jumpifzero"}, "For every two items in the input sequence, first subtract the second from the first and put the result in the output sequence, then subtract the first from the second and put the result in the output sequence, and repeat."},
    {8, {6, 2, 7, 7, -9, 3, -3, -3}, 2, {7, -3}, 3, 8, {"inbox", "outbox", "copyfrom", "copyto", "add", "sub", "jump", "jumpifzero"}, "Take two numbers from the input sequence in sequence, output one if they are equal, otherwise throw it away. Repeat this process until the input conveyor belt is empty."}
};

void pre1();
void pre234();

void pre() {
    cin >> level; // 获取有效的关卡输入

    switch (level) {
        case 1: pre1(); break;
        case 2: pre234();break;
		case 3: pre234(); break;
    }
}

void robotmove(string instructionname, int instructionIndex) {
    if (instructionname == "outbox") {
        if (currentblock.ifhave == 0 && ifcout == 0) {
            ifnormalend = 0;
            cout << "Error on instruction " << instructionIndex;
            ifcout = 1;
        } else {
            outboxshuzu[outboxshuzuxiabiao] = currentblock.num;
            outboxshuzuxiabiao++;
            currentblock.ifhave = false;
            currentblock.num = 0;
        }
    } else if (instructionname == "inbox") {
        currentblock.ifhave = 1;
        currentblock.num = levelinfo[level - 1].geichuinboxshuzu[geichuinboxshuzuxiabiao];
        geichuinboxshuzuxiabiao++;
    } else if (instructionname == "add") {
        if (currentblock.ifhave == 0 || blankspace[instructionNum.num].ifhave == 0) {
            cout << "Error on instruction " << instructionIndex;
        } else {
            currentblock.num += blankspace[instructionNum.num].num;
        }
    } else if (instructionname == "sub") {
        if (currentblock.ifhave == 0 || blankspace[instructionNum.num].ifhave == 0) {
            cout << "Error on instruction " << instructionIndex;
        } else {
            currentblock.num -= blankspace[instructionNum.num].num;
        }
    } else if (instructionname == "copyto") {
        if (currentblock.ifhave == 0) {
            cout << "Error on instruction " << instructionIndex;
        } else {
            blankspace[instructionNum.num].ifhave = 1;
            blankspace[instructionNum.num].num = currentblock.num;
        }
    } else if (instructionname == "copyfrom") {
        if (blankspace[instructionNum.num].ifhave == 0) {
            cout << "Error on instruction " << instructionIndex;
        } else {
            currentblock.num = blankspace[instructionNum.num].num;
            currentblock.ifhave = 1;
        }
    }
}

int main() {
    pre();
    return 0;
}

void inputofinstructions() {
    cin >> instructionnum;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 1; i <= instructionnum; ++i) {
        getline(cin,instruction[i].name);
    }
}

bool isValidInteger(const string& str) {
    if (str.empty()) return false;
    for (char ch : str) {
        if (!isdigit(ch)) return false;
    }
    return true;
}

void pre1() {
    inputofinstructions();
    int i = 1;
    while (i) {
        if (instruction[i].name == "inbox" && geichuinboxshuzuxiabiao >= levelinfo[level - 1].inboxnum) {
            break;
        } else if (i == instructionnum + 1) break;

        if (instruction[i].name == "inbox" || instruction[i].name == "outbox") {
            robotmove(instruction[i].name, i);
            i++;
        } else {
            ifnormalend = 0;
            break;
        }
    }
    if (ifnormalend == 0) {
        if (ifcout == 0) {
            ifcout = 1;
            cout << "Error on instruction " << i;
        }
        return;
    }
    bool iftrulysyccess = 1;
    for (int i = 0; i < levelinfo[level - 1].outboxnum; ++i) {
        if (outboxshuzu[i + 1] != levelinfo[level - 1].yuqishuchu[i]) {
            iftrulysyccess = 0;
            break;
        }
    }
    if (iftrulysyccess == 0 || outboxshuzuxiabiao - 1 != levelinfo[level - 1].outboxnum) {
        cout << "Fail";
    } else {
        cout << "Success";
    }
}

void pre234() {
    inputofinstructions();
    int i = 1;
    while (true) {
        if (instruction[i].name == "inbox" && geichuinboxshuzuxiabiao >= levelinfo[level - 1].inboxnum) {
            break;
        } else if (i == instructionnum + 1) break;

        instructionNum = {};
        size_t pos = instruction[i].name.find(' ');
        string instructionName = instruction[i].name;
        if (pos != string::npos) {
            string operand = instruction[i].name.substr(pos + 1);
            if (isValidInteger(operand)) {
                instructionNum.ifhave = 1;
                instructionNum.num = stoi(operand);
                instructionName = instruction[i].name.substr(0, pos);

                if (instructionName == "jump" || instructionName == "jumpifzero") {
                    if (instructionNum.num < 1 || instructionNum.num > instructionnum || instructionNum.num == i) {
                        if (ifcout == 0) {
                            cout << "Error on instruction " << i;
                            ifcout = 1;
                        }
                        return;
                    }
                } else if (instructionName == "add" || instructionName == "sub" || instructionName == "copyto" || instructionName == "copyfrom") {
                    if (instructionNum.num < 0 || instructionNum.num > 2) {
                        if (ifcout == 0) {
                            cout << "Error on instruction " << i;
                            ifcout = 1;
                        }
                        return;
                    }
                }
            } else {
                if (ifcout == 0) {
                    cout << "Error on instruction " << i;
                    ifcout = 1;
                }
                return;
            }
        }

        if ((instructionName == "inbox" || instructionName == "outbox") && instructionNum.ifhave == 0) {
            instruction[i].ifused++;
            robotmove(instructionName, i);
            i++;
        } else if (instructionName == "jump" && instructionNum.ifhave == 1) {
            instruction[i].ifused++;
            i = instructionNum.num;
        } else if (instructionName == "jumpifzero" && instructionNum.ifhave == 1) {
            if (currentblock.ifhave == 0) {
                ifnormalend = 0;
                break;
            }
            if (currentblock.num == 0) {
                instruction[i].ifused++;
                i = instructionNum.num;
            } else {
                instruction[i].ifused++;
                i++;
            }
        } else if ((instructionName == "add" || instructionName == "sub" || instructionName == "copyto" || instructionName == "copyfrom") && instructionNum.ifhave == 1) {
            instruction[i].ifused++;
            robotmove(instructionName, i);
            i++;
        } else {
            ifnormalend = 0;
            break;
        }
    }
    if (ifnormalend == 0) {
        if (ifcout == 0) {
            cout << "Error on instruction " << i;
            ifcout = 1;
        }
        return;
    }
    bool iftrulysyccess = 1;
    for (int i = 0; i < levelinfo[level - 1].outboxnum; ++i) {
        if (outboxshuzu[i + 1] != levelinfo[level - 1].yuqishuchu[i]) {
            iftrulysyccess = 0;
            break;
        }
    }
    if (iftrulysyccess == 0 || outboxshuzuxiabiao - 1 != levelinfo[level - 1].outboxnum) {
        cout << "Fail";
    } else {
        cout << "Success";
    }
}
