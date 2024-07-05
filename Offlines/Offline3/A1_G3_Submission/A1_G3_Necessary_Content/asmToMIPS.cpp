#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<unordered_map>
#include<map>
#include<string>
#include<algorithm>
#include<vector>
using namespace std;

unordered_map<string, pair<int, char> > insMap;
unordered_map<string, int> regMap;
map<string,int> labelMap;
vector<string> inp;

int lineCount = 1;

void loadRegisters() {
    //zero t0 t1 t2 t3 t4 sp
    regMap["$zero"] = 0x0;
    regMap["$t0"] = 0x1;
    regMap["$t1"] = 0x2;
    regMap["$t2"] = 0x3;
    regMap["$t3"] = 0x4;
    regMap["$t4"] = 0x5;
    regMap["$sp"] = 0x6;
}

void loadInstructions() {
    //EKIM NHPG OFLA JDBC
    insMap["and"] = make_pair(0x0, 'R');
    insMap["nor"] = make_pair(0x1, 'R');
    insMap["sll"] = make_pair(0x2, 'S');
    insMap["sw"] = make_pair(0x3, 'I');
    insMap["beq"] = make_pair(0x4, 'S');
    insMap["ori"] = make_pair(0x5, 'S');
    insMap["j"] = make_pair(0x6, 'J');
    insMap["or"] = make_pair(0x7, 'R');
    insMap["bneq"] = make_pair(0x8, 'S');
    insMap["andi"] = make_pair(0x9, 'S');
    insMap["lw"] = make_pair(0xA, 'I');
    insMap["add"] = make_pair(0xB, 'R');
    insMap["srl"] = make_pair(0xC, 'S');
    insMap["subi"] = make_pair(0xD, 'S');
    insMap["addi"] = make_pair(0xE, 'S');
    insMap["sub"] = make_pair(0xF, 'R');
    insMap["push"] = make_pair(0x10, 'I');
    insMap["pop"] = make_pair(0x11, 'I');
}

void rType(string s) {
    char opCode[10];
    char src1[10];
    char src2[10];
    char dest[10];
    sscanf(s.c_str(), "%6[^ ] %6[^, ] , %6[^, ] , %6[^, ]", opCode,dest,src1,src2);
    //cout<<opCode<<" "<<src1<<" "<<src2<<" "<<dest<<endl;
    unsigned char msb = insMap[opCode].first;
    unsigned char lsb = regMap[src1];
    unsigned char temp;
    msb = (msb<<4) | lsb;
    temp = (unsigned char) msb;
    cout<<temp;
    msb = regMap[src2];
    lsb = regMap[dest];
    //cout<<endl<<regMap[dest]<<endl;
    //cout<<msb<<"..."<<lsb<<endl;
    msb = (msb<<4 | lsb);
    temp = (unsigned char) msb;
    //cout<<hex<<msb<<endl;
    cout<<temp;
}

bool isNumber(string& s) {
    //cout<<"--------------"<<s<<endl;
    for(int i =0; i<s.length(); i++) {
    //    cout<<"Here ----"<<s[i]<<endl;
    //cout<<"Here --------"<<s.length()<<endl;
        if(s[i] > '9' || s[i] <'0') {
            return false;
        }
    }
    return true;
}

void sType(string s) {
    char opCode[10];
    char src1[10];
    char dest[10];
    char shamt[10];
    sscanf(s.c_str(), "%6[^ ] %6[^, ] , %6[^, ] , %6[^, ]", opCode,dest,src1,shamt);
    unsigned char msb = (unsigned char) insMap[opCode].first;
    unsigned char lsb = (unsigned char) regMap[src1];
    msb = msb<<4 | lsb;
    cout<<msb;
    string temp(shamt);
    //cout<<"****"<<temp<<endl;
    //cout<<"*******"<<isNumber(temp)<<endl;
    bool f = isNumber(temp);
    //cout<<"**"<<atoi(temp.c_str())<<"**"<<f<<endl;
    //cout<<"\nDebug : "<<temp<<" "<<labelMap[temp]<<" "<<lineCount<<endl;
    lsb = f ? (unsigned char)atoi(temp.c_str()) : (unsigned char)(labelMap[temp]-lineCount-1);
    //cout<<"**"<<(lsb & 0x0f)<<endl;
    msb = (unsigned char) regMap[dest];
    msb = (msb<<4) | (lsb & 0x0f);
    cout<<msb;
}

void iType(string s) {
    char opCode[10];
    char src1[10];
    char dest[10];
    int addr;
    sscanf(s.c_str(), "%6[^ ] %6[^, ] , %d(%6[^, )])", opCode,dest,&addr,src1);
    unsigned char msb = (unsigned char) insMap[opCode].first;
    unsigned char lsb = (unsigned char) regMap[src1];
    msb = (msb<<4) | lsb;
    cout<<msb;
    msb = (unsigned char) regMap[dest];
    lsb = (unsigned char) addr;
    msb = (msb<<4) | lsb;
    cout<<msb;
}

void jType(string s) {
    char opCode[10];
    char jump[12];
    sscanf(s.c_str(), "%6[^ ] %10[^, ]", opCode,jump);
    unsigned char msb = (unsigned char) insMap[opCode].first;
    unsigned char lsb = (unsigned char) labelMap[jump];
    msb = (msb<<4) | (lsb>>4);
    cout<<msb;
    lsb = (lsb<<4);
    cout<<lsb;
}

string extractOpcode(string &s) {
    string opCode = "";
    int i = 0;
    while(i<s.length() && s[i] != ' ') {
        opCode += s[i];
        i++;
    }
    //cout<<"OPCODE "<<opCode<<endl;
    return opCode;
}

void handlePushPop(string newCmd) {
    if(newCmd == "") {
        return;
    }
    string opCode = extractOpcode(newCmd);
    string reg = "";
    int i = opCode.length()+1;
    while(i < newCmd.length() && newCmd[i] != ' ') {
        reg += newCmd[i++];
    }
    //cout<<"debug --- "<<newCmd<<" "<<reg<<" "<<opCode<<endl;
    if(opCode == "push") {
        lineCount++;
        string x = "sw "+reg+",0($sp)";
        string y = "subi $sp,$sp,1";
        inp.push_back(x);
        inp.push_back(y);
    }
    else if(opCode == "pop") {
        lineCount++;
        string x = "lw "+reg+",0($sp)";
        string y = "addi $sp,$sp,1";
        inp.push_back(y);
        inp.push_back(x);
    }
    else {
        inp.push_back(newCmd);
    }
}

void labelParse(string cmd) {
    string opCode = extractOpcode(cmd);
    //cout<<"Opcode ------->>> "<<opCode<<endl;
    auto p = insMap.find(opCode);
    if(p == insMap.end()) {
        int idx = opCode.length();
        string label = "";
        for(int i=0;i<idx-1;i++) {
            label += opCode[i];
        }
        labelMap[label] = lineCount;
        string newCmd = "";
        idx++;
        while(idx < cmd.length()) {
            newCmd += cmd[idx++];
        }
        //push pop
        handlePushPop(newCmd);
    }
    else {
        //push pop
        handlePushPop(cmd);
    }
}

void parse(string s) {
    string opCode = extractOpcode(s);
    auto p = insMap.find(opCode);
    if(p->second.second == 'R') {
        rType(s);
    }
    else if(p->second.second == 'S') {
        //cout<<"here "<<lineCount<<"\nhey\n";
        sType(s);
    }
    else if(p->second.second == 'I') {
        iType(s);
    }
    else if(p->second.second == 'J'){
        jType(s);
    }
}

int main() {
    freopen("asm.txt", "r", stdin);
    freopen("mips.txt", "w", stdout);
    loadInstructions();
    loadRegisters();

    string inputStr;
    string stackPos = "addi $sp, $sp, 15";
    inp.push_back(stackPos);
    while(getline(cin, inputStr)) {
        if(inputStr == "") {
            continue;
        }
        lineCount++;
        labelParse(inputStr);
    }
    // for(auto s: inp) {
    //     cout<<s<<endl;
    // }
    lineCount = 0;
    for(auto s: inp) {
        lineCount++;
        //cout<<s<<endl;
        parse(s);
    }
}
