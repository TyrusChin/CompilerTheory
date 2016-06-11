#include <fstream>
#include <iostream>
#include "../header/error_process.h"
using namespace std;
void pretreatment(char filename[], char buf[], char target[]){

    ifstream cinf(filename, ios::in);   // 定义输入流，从文件输入到内存
    if(!cinf.is_open()){
        err_exit((char *)"文件不存在！");
    }
    char c0 = '$', c1;  // 记录前一个字符，用于处理一些注释和续行符
    bool in_comment = false;    // 注释标记
    bool in_string = false;     // 字符串标记
    char string_flag = '"';     // PHP的字符串定义方法有两种，""和''，需要先留下这个标记，便于判断结束
    bool clear_c1 = false;      // 在字符串中出现连续两个\\的情况，需要将c1置为非\，否则会判断失败
    int comment_type = 0;       // 注释类型，1->/**/ 2->//

    cout << "预处理PHP源文件中... " << endl;
    cout << "<源程序>" << endl;

    int i = 0;
    void *p = cinf.read(&c1, sizeof(char));  // 每次读一个字符
    while(p){
        cout << c1; // 应该注释掉? 输出读入字符
        if(false == in_comment){
            // 不在注释中

            if(true == in_string){
                // 在字符串中，不考虑注释判断
                clear_c1 = false;
                if(string_flag == '"') {
                    // 双引号作为定义符
                    if (c0 == '\\' && c1 == '\\') {
                        // 配对成一个\，于是将c1置为非\的符号，防止出现如"\\""这样的语法错误
                        clear_c1 = true;
                    }
                }else if(string_flag == '\''){
                    // 单引号作为定义符，不同的是在字符串内部，除了\'之外，不识别任何转义
                    // 我们定义处理的时候需要把字符串定义符的单引号转成双引号，并将其中的\'替换成'，其余的\替换成\\，防止意外转义
                    if(c0 == '\\'){
                        if(c1 == '\''){
                            --i;    // 去除已存入的\。
                        }else{
                            buf[i++] = '\\';    // 增加一个\。
                        }
                    }
                }else{
                    err_exit((char *)"内部错误！非法的字符串定义符！");
                }
                if (c0 != '\\') {
                    // 字符没有被转义的情况下
                    if (string_flag == c1) {
                        // 如果碰到当前符号与字符串定义符号相同，意味着字符串到了尽头
                        in_string = false;
                        if(string_flag == '\''){
                            // '作为字符串定义符的时候，要转成"
                            c1 = '"';
                        }
                    }
                }
                buf[i++] = c1;  // 将字符存入扫描缓冲区
                if(true == clear_c1){
                    c1 = ' ';
                }
            }else{
                // 不在字符串中
                // 字符串定义符为'和"，这里将'转成"。
                if(c1 == '"' || c1 == '\''){
                    in_string = true;
                    string_flag = c1;
                    c1 = '"';
                }

                // PHP的注释是 // 和 /* */
                if(c0 == '/' && c1 == '*'){
                    // 进入注释，去掉已存入扫描缓冲区的字符\。
                    in_comment = true;
                    comment_type = 1;
                    --i;
                }else if(c0 == '/' && c1 == '/'){
                    in_comment = true;
                    comment_type = 2;
                    --i;
                }else if(c0 == '\\' && c1 == '\n'){
                    --i;    // 去掉续行符
                }else{
                    if(c1 == '\t' || c1 == '\r'){
                    // if(c1 == '\t' || c1 == '\n' || c1 == '\r'){
                        c1 = ' ';
                    }
                    buf[i++] = c1;  // 将字符存入扫描缓冲区
                }
            }
        }else{
            // 在注释中
            if(comment_type == 1 && c0 == '*' && c1 == '/'){
                in_comment = false;
            }else if(comment_type == 2 && c1 == '\n'){
                in_comment = false;
                // buf[i++] = ' ';
            }
            if(c1 == '\n'){
                buf[i++] = '\n';
            }
        }
        c0 = c1;    // 向前读一个字符
        p = cinf.read(&c1,sizeof(char));
    }
    buf[i] = '#';

    ofstream coutf(target);
    coutf << buf;
    cout << "预处理PHP结束，结果保存在 '" << target << "' 中。" << endl;
    return ;
}
