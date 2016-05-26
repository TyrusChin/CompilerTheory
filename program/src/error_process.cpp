#include <iostream>
using namespace std;

void err_exit(char str[]){
    cerr << str << endl;
    exit(0);
}

void msg_notice(char str[]){
    cout << str << endl;
    exit(0);
}
