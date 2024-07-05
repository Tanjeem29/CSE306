#include<iostream>
#include<fstream>
#include<iomanip>
using namespace std;

int main() {
    FILE* in = fopen("mips.txt", "rb");
    freopen("readableMIPS.txt", "w", stdout);
    int byte;
    while((byte = getc(in)) != EOF) {
        cout<<hex<<setw(2)<<setfill('0')<<(int)byte;
        cout<<hex<<setw(2)<<setfill('0')<<(int)getc(in)<<" ";
    }
}
