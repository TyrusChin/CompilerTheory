#include <iostream>
#include <string.h>
#include "header/pretreatment.h"
#include "header/lex.h"
#include "header/recursive_descent.h"

using namespace std;

// 放置程序的数组
char buf[4048] = {'\0'};

int main(int argc, char* argv[]) {

    // argv[0]表示命令，从1开始才是参数
    // -p 预处理，-l 词法分析，没有参数或-c则表示语法分析

    while(1) {
        string name;
        cout << "请输入测试文件的名字(不含后缀)，输入exit表示退出：" << endl;
        cin >> name;
        if("exit" == name){
            break;
        }

        // 清空buffer
        memset(buf, 0, sizeof(char)*4048);

        string data_dir = "/home/tyruschin/ClionProjects/CompilerTheory/program/data/example/";
        string res_dir = "/home/tyruschin/ClionProjects/CompilerTheory/program/data/res/";
        string source_file = name + ".php";
        string pretreatment_target_file = name + ".pre.php";
        string lex_target_file = name + ".lex.php";

        if (1 == argc || (2 == argc && 0 == strcmp("-c", argv[1]))) {
            // 预处理
            pretreatment((char *) ((data_dir + source_file).c_str()), buf,
                         (char *) ((res_dir + pretreatment_target_file).c_str()));

            // 词法分析
            lex(buf, (char *) ((res_dir + lex_target_file).c_str()));

            // 语法分析
            parser((char *) (res_dir + lex_target_file).c_str());
        } else {

            if (0 == strcmp("-p", argv[1])) {
                pretreatment((char *) ((data_dir + source_file).c_str()), buf,
                             (char *) ((res_dir + pretreatment_target_file).c_str()));
            }

            if (0 == strcmp("-l", argv[1])) {
                pretreatment((char *) ((data_dir + source_file).c_str()), buf,
                             (char *) ((res_dir + pretreatment_target_file).c_str()));
                lex(buf, (char *) ((res_dir + lex_target_file).c_str()));
            }
        }
    }

    return 0;
}