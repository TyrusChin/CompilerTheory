#include <string>
#include <fstream>
#include <iostream>
#include "../header/error_process.h"
#define MROW 47
#define MCOL 29
using namespace std;

// 后面要用的一个函数
int is_int(string s){
    for(int i = 0; i < s.size(); i++){
        if(s[i] <'0' || s[i] > '9'){
            return 0;
        }
    }
    return 1;
}

// (code, val)
struct code_val{
    string code;
    string val;
};

// 字符集
// 一定要按照重新标记过的顺序，这样才能和状态转移矩阵对接
const string col_char = "$0.+-\"Aa=<?*/>%!()[]{},;#_:\20E"; // 0代表0~9，Aa分别代表大小写，\20代表的是空格，E代表其他字符

// 状态转移矩阵
int M[MROW][MCOL] = {
        {1,2,3,4,5,6,7,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,7,46},
        {0,0,0,0,0,0,25,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25},
        {0,26,27},
        {0,28},
        {0,30,0,29},
        {0,30,0,0,31},
        {34,34,34,34,34,32,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34},
        {0,0,0,0,0,0,33,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33},
        {0,0,0,0,0,0,0,0,35},
        {0,0,0,0,0,0,0,0,36,0,37},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,38},
        {0},
        {0},
        {0,0,0,0,0,0,0,0,39},
        {0},
        {0,0,0,0,0,0,0,0,40},
        {0},
        {0},
        {0},
        {0},
        {0},
        {0},
        {0},
        {0},
        {0},
        {0,41,0,0,0,0,41,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41},
        {0,26,27},
        {0,42},
        {0,42},
        {0},
        {0,43,44},
        {0},
        {0},
        {0,0,0,0,0,0,33,33},
        {34,34,34,34,34,32,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34},
        {0},
        {0},
        {0},
        {0},
        {0},
        {0},
        {0,41,0,0,0,0,41,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41},
        {0,42},
        {0,43,44},
        {0,45},
        {0,45}
};

// 是否属于终态集
int is_final_state(int state){
    int final[MROW];
    for(int i = 0; i < MROW; i ++){
        final[i] = 1;
    }
    final[0] = 0;
    final[1] = 0;
    final[6] = 0;
    final[10] = 0;
    final[34] = 0;
    return final[state];
}

// 列定位函数，传入一个字符，返回对应的列号
int col(char c, const string s){
    if(c >= 'a' && c <= 'z'){
        c = 'a';    // 小写
    }else if(c >= 'A' && c <= 'Z'){
        c = 'A';    // 大写
    }else if(c >= '0' && c <= '9'){
        c = '0';    // 数字
    }
    for(int i = 0; i < s.size() - 1; i++){
        // s[s.size() - 1]为E，代表其他字符
        if(c == s[i]){
            return i;
        }
    }
    // 属于其他字符的类型，但不会结束，因为在字符串中可以接受
    return (int)(s.size() - 1);
}

// 单词编码表
string search_table(string token){
    // 可以判断出除了 字符串 变量 数字类型 之外的其他单词
    string table[] = {
            "<?", "?>", "function", "break", "continue", "switch", "case", "default", "if", "else", "elseif", "echo", "while", "for", "return", "+", "-", "*", "/", "%", "=", "(", ")", "[", "]", "{", "}", "++", "--", "#", ",", ".", "==", ">", ">=", "<", "<=", "!=", "!", ";", ":"
    };
    string code[] = {
            "bg", "ed", "fn", "br", "ct", "sw", "cs", "df", "if", "es", "ef", "ec", "wh", "fr", "rt", "+", "-", "*", "/", "%", "=", "(", ")", "[", "]", "{", "}", "++", "--", "#", ",", ".", "==", ">", ">=", "<", "<=", "!=", "!", ";", ":"
    };
    int size = sizeof(table)/sizeof(string *);
    for(int i = 0; i < size; i++){
        if(table[i] == token){
            return code[i];
        }
    }
    return "";
}

// 单词二元式扫描函数
struct code_val scanner(char *buf, int &i){
    // 每调用一次，返回一个单词的二元式
    struct code_val res = {"","NUL"};
    string token = "";  // 用于保存单词，有意义的时候，填写到res的val字段中
    int state = 0, current_final_state = -1, j = col(buf[i], col_char);   // 初始状态为0，当前的所处的最长匹配(处于终态的情况)记录为current_final_state，获得当前的字符对应的列号
    while(M[state][j]){
        // 循环直到没有后继状态
        token += buf[i];
        if(buf[i] == '#'){
            current_final_state = 24;   // #的状态是24，事实上只要current_final_state不是-1就判断通过
            break;
        }
        state = M[state][j];
        if(is_final_state(state)){
            current_final_state = state;
        }
        j = col(buf[++i], col_char);  // 将下一个字符转换成列号
        // 特殊情况：在字符串中出现"\""的情况，为了转义的"不被识别成字符串定义符，需要修改j的值
        if(token[0] == '"'){
            if(buf[i] == '"' && buf[i-1] == '\\'){
                j = (int)(col_char.size() - 1); // 把\"归类为其他字符
            }
        }
    }
    if(current_final_state == -1){
        cout << token << "--" << state << endl;
        // 不能到达终态，出错！
        err_exit((char *)"内部错误！请检查是否有不正确的转义字符！");
    }
    res.code = search_table(token);
    if(res.code == ""){
        // 可能是字符串、变量、函数名、数字
        if(token[0] == '"'){
            res.code = "s";
            res.val = token.substr(1,token.size() - 2); // 去掉前后两个双引号
        }else if(token[0] == '$'){
            res.code = "id";
            res.val = token.substr(1,token.size() - 1); // 去掉$
        }else if(token[0] == '_' || (token[0] >= 'a' && token[0] <= 'z') || (token[0] >= 'A' && token[0] <= 'Z')){
            res.code = "fs";
            res.val = token;
        }else{
            // 寻找是否有+/-
            if((token.find("+") != token.npos) || (token.find("-") != token.npos)){
                // 有符号
                if(token.find(".") != token.npos){
                    // 浮点数
                    res.code = "f";
                }else{
                    // 整数
                    res.code = "i";
                }
            }else{
                // 无符号
                if(token.find(".") != token.npos){
                    // 浮点数
                    res.code = "uf";
                }else if(is_int(token)){
                    // 整数
                    res.code = "ui";
                }else{
                    err_exit((char *)"内部错误！非法的单词被识别！");
                }
            }
            res.val = token;
        }
    }
    return res;
}

void printM(){
    for(int i = 0; i < MROW; ++i){
        for (int j = 0; j < MCOL; ++j) {
            cout << M[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// 对外接口，循环调用code_val
void lex(char buf[], char target[]){

    // print 矩阵
    // printM();exit(0);

    // buf为预处理后的结果，target为词法分析的结果的保存路径

    ofstream coutf(target, ios::out);
    code_val t;
    int i = 0;

    // 以下两个变量只是用于格式化显示
    int j = 0;
    int line_print = 5;

    cout << "正在对预处理后的PHP文件进行词法分析中... " << endl;
    cout << "<单词二元式>" << endl;

    do{
        while(buf[i] == ' '){
            ++i;
        }
        t = scanner(buf, i);
        if((j++)%line_print == 0){
            cout << endl;
        }
        cout << '(' << t.code << ',' << t.val << ')';
        coutf << t.code << '\t' << t.val << endl;

    }while(t.code != "#");
    cout << endl << "词法分析结束，结果保存在 '" << target << "' 中。" << endl;
    return ;
}

