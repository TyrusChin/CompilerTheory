#include <iostream>
#include "header/pretreatment.h"
#include "header/lex.h"
#include "header/recursive_descent.h"

using namespace std;

int main() {

    string name;
    cout << "请输入测试文件的名字(不含后缀)：" << endl;
    cin >> name;

    // 放置程序的数组
    char buf[4048] = {'\0'};

    string data_dir = "/home/tyruschin/ClionProjects/CompilerTheory/program/data/";
    string source_file = name + ".php";
    string pretreatment_target_file = name + ".pre.php";
    string lex_target_file = name + ".lex.php";

    // 预处理
    pretreatment((char *)((data_dir+source_file).c_str()), buf, (char *)((data_dir+pretreatment_target_file).c_str()));
    cout << "<预处理结果>" << endl;
    cout << buf << endl;

    // 词法分析
    lex(buf, (char *)((data_dir+lex_target_file).c_str()));
    
    // 语法分析
    parser((char *)(data_dir+lex_target_file).c_str());
    

    return 0;
}