#ifndef PROGRAM_ERROR_PROCESS_H
#define PROGRAM_ERROR_PROCESS_H
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

#endif //PROGRAM_ERROR_PROCESS_H
