#include <iostream>
#include <string>
#include "header/pretreatment.h"

using namespace std;

int main() {
    char buf[4048] = {'\0'};

    string data_dir = "/home/tyruschin/ClionProjects/CompilerTheory/program/data/";
    string source_file = "test1.php";
    string pretreatment_target_file = "test1.pre.php";

    pretreatment((char *)((data_dir+source_file).c_str()), buf, (char *)((data_dir+pretreatment_target_file).c_str()));
    cout << "<预处理结果>" << endl;
    cout << buf << endl;
    return 0;
}