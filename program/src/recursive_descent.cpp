#include <fstream>
#include <iostream>
#include "../header/word_code.h"
#include <queue>
#include <stack>
using namespace std;

// 函数原型说明，每个产生式都对应一个
void P();   // 程序
void B();   // 代码块
void S();   // 语句
void CS();  // 条件语句
void PS();  // 过程语句
void SS();  // switch语句
void RV();  // 右值
void AS();  // 赋值语句
void MS();  // 算术语句
void EF();  // elseif语句
void ES();  // else语句
void AE();  // 赋值表达式
void VAR(); // 变量
void NM();  // 数字
void STR(); // 字符串
void FC();  // 函数调用
void ME();  // 算术表达式
void MO();  // 算术运算符
void IDX(); // 索引
void CP();  // 调用函数参数

struct code_val {
    string code;
    string val;
};

int forward_word(); // 声明
char tval[1024] = {'\0'};

// 全局的单词结构，指向当前的处理单词
code_val t;
ifstream cinf;
// 用于解决冲突的队列
queue<code_val> qc;
// 用于解决大小括号匹配的栈
stack<string> sm;

// 错误输出函数 debug
//void parse_error(string func, string code = t.code){
//    cout << endl << "Error in " << func << "->" << code << endl;
//    exit(0);
//}

// 错误输出函数 正式的
void parse_error(string reason, string code = t.code){
    cout << endl << reason << " : " << code << endl;
    exit(0);
}

// 主函数
void parser(char code_val_file[]){
    cinf.open(code_val_file, ios::in);
    forward_word();
    P();
    if(t.code == "#" && sm.empty()){
        cout << endl << "语法正确" << endl;
    }else{
        parse_error("多余的符号");
    }
    return ;
}

int str_in_array(string find, string array[], int size){
    // 判断find的字符串在数组中是否存在，存在则返回1，不存在则返回0
    // size 是 array 的大小
    for(int i = 0; i < size; i++){
        if(find == array[i]){
            return 1;
        }
    }
    return 0;
}

int forward_word(){
    // 移进一个单词
    if(qc.empty()) {
        cinf >> t.code;
        cinf.getline(tval, sizeof(tval));
        t.val = tval;
        cout << t.code << " " << t.val << endl;
    }else{
        t = qc.front();
        qc.pop();
    }
    return 1;
}

// 匹配括号
// 调用时机：以下的情况所对应的函数，如STR()，函数体内第一句和最后一句
// 支持大括号的位置：B CS PS SS AS
// 支持小括号的位置：RV NM ME STR VAR FC MS AE
int match_brackets(string symbol){
    // 传入{}() 的其中一种，循环压栈。
    while (symbol == t.code) {
        if (sm.empty()) {
            if (")" == symbol || "}" == symbol) {
                return 0;
            }
            sm.push(symbol);
            forward_word();
        } else {
            string top = sm.top();
            if (")" == symbol && "(" == top) {
                sm.pop();
            } else if ("}" == symbol && "{" == top) {
                sm.top();
            } else if ("(" == symbol || "{" == symbol) {
                sm.push(symbol);
                forward_word();
            } else {
                return 0;
            }
        }
    }
    return 1;
}

void P(){
    // P -> (bg B ed P) | ε
    // first(P) = {bg, ε}
    // follow(P) = {#}
    if(BG == t.code){
        forward_word();
        B();
        if(ED == t.code){
            forward_word();
            P();
        }
    }else if("#" != t.code){
        // ≠ bg/#
        parse_error("缺少<?开始符号，或者非法的结束符");
    }
    // 如果是#的情况，则处理成空字的情况
}

void B(){
    if(!match_brackets("{")){
        parse_error("非法的语句开始符号");
    }
    // B -> (S B) | ε
    // first(B) = {if , fn , wh , sw , fr , br , ct , ec , rt , id, ! , ++ , -- , s , i , ui , f , uf , fs , ε}
    // follow(B) = {ed , } , cs , df}
    string arr1[] = {IF, FN, WH, SW, FR, BR, CT, EC, RT, ID, "!", "++", "--", _S, I, UI, F, UF, FS};
    string arr2[] = {ED, _CS, DF, "}"};
    if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        S(), B();
    }else if(!str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
        parse_error("非法的语句开始符号");
    }
    if(!match_brackets("}")){
        // parse_error("B3");
    }
}

void S(){
    // S -> CS | PS
    // first(S) = {if , fn , wh , sw , fr , br , ct , ec , rt , id , ! , ++ , -- , s , i , ui , f , uf , fs}
    // first(CS) = {if , fn , wh , sw , fr}
    // first(PS) = {br , ct , ec , rt , id , ! , ++ , -- , s , i , ui , f , uf , fs}
    // 由于不能推出空字，不需要考虑follow集
    string arr1[] = {IF, FN, WH, SW, FR};
    string arr2[] = {BR, CT, EC, RT, ID, "!", "++", "--", _S, I, UI, F, UF, FS};
    if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        CS();
    }else if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
        PS();
    }else{
        parse_error("非法的语句开始符号");
    }
}

void CS(){
    if(!match_brackets("{")){
        parse_error("非法的语句开始符号");
    }
    /*
     * CS -> fn fs '(' AS ')' { B }
     * CS -> wh '(' (RV | AS) ')' ({ B } | S)
     * CS -> sw '(' RV ')' { SS }
     * CS -> fr '(' (AS | ε) ; (RV | ε) ; (MS | ε) ')' ({ B } | S)
     * CS -> if '(' (RV | AS) ')' ( {B} | S ) EF ES
     * */
    // first(CS) = {if , fn , wh , sw , fr}
    // first(RV) = {id , ! , ++ , -- , s , i , ui , f , uf , fs}
    // first(AS) = {id}
    // first(MS) = {id , ! , ++ , -- , s , i , ui , f , uf}
    // first(S) = {if , fn , wh , sw , fr , br , ct , ec , rt , id}
    int match = 0;
    string arr1[] = {"!" , "++" , "--" , _S , I , UI , F , UF , FS};
    string arr2[] = {"," , "="};
    string arr3[] = {IF, FN, WH, SW, FR , BR, CT, EC, RT, ID};
    string arr4[] = {ID, "!", "++", "--", _S, I , UI , F , UF , FS};
    string arr5[] = {ID, "!", "++", "--", _S, I , UI , F , UF};
    if(FN == t.code){
        forward_word();
        if(FS == t.code){
            forward_word();
            if("(" == t.code){
                forward_word();
                AS();
                if(")" == t.code){
                    forward_word();
                    if("{" == t.code){
                        forward_word();
                        B();
                        if("}" == t.code){
                            forward_word();
                            match = 1;
                        }
                    }
                }
            }
        }
    }else if(WH == t.code){
        forward_word();
        if("(" == t.code){
            // 解决冲突问题，合并左因子
            forward_word();
            if(ID == t.code){
                // 冲突，用队列来解决
                code_val cv1 = t;
                forward_word();
                qc.push(cv1);
                qc.push(t);
                if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
                    forward_word();
                    AS();
                }else{
                    forward_word();
                    RV();
                }
            }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
                RV();
            }else{
                parse_error("while语句语法不正确");
            }
            if(")" == t.code) {
                forward_word();
                if("{" == t.code){
                    forward_word();
                    B();
                    if("}" == t.code){
                        forward_word();
                        match = 1;
                    }
                }else if(str_in_array(t.code, arr3, sizeof(arr3)/sizeof(arr3[0]))){
                    S();
                    match = 1;
                }
            }
        }
    }else if(SW == t.code){
        forward_word();
        if("(" == t.code){
            forward_word();
            RV();
            if(")" == t.code){
                forward_word();
                if("{" == t.code){
                    forward_word();
                    SS();
                    if("}" == t.code){
                        forward_word();
                        match = 1;
                    }
                }
            }
        }
    }else if(FR == t.code){
        forward_word();
        if("(" == t.code){
            forward_word();
            if("id" == t.code){
                AS();
            }
            if(";" == t.code){
                forward_word();
                if(str_in_array(t.code, arr4, sizeof(arr4)/sizeof(arr4[0]))){
                    RV();
                }
                if(";" == t.code){
                    forward_word();
                    if(str_in_array(t.code, arr5, sizeof(arr5)/sizeof(arr5[0]))){
                        MS();
                    }
                    if(")" == t.code){
                        forward_word();
                        if("{" == t.code){
                            forward_word();
                            B();
                            if("}" == t.code){
                                forward_word();
                                match = 1;
                            }
                        }else if(str_in_array(t.code, arr3, sizeof(arr3)/sizeof(arr3[0]))){
                            S();
                            match = 1;
                        }
                    }
                }
            }
        }
    }else if(IF == t.code){
        forward_word();
        if("(" == t.code) {
            forward_word();
            if(ID == t.code){
                // 冲突，用队列来解决
                code_val cv1 = t;
                forward_word();
                qc.push(cv1);
                qc.push(t);
                if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
                    forward_word();
                    AS();
                }else{
                    forward_word();
                    RV();
                }
            }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
                RV();
            }else{
                parse_error("if语句的条件判断语法不正确");
            }
            if(")" == t.code) {
                forward_word();
                if("{" == t.code){
                    forward_word();
                    B();
                    if("}" == t.code){
                        forward_word();
                    }else{
                        parse_error("if语句的语法不正确，可能缺少一些必要的界符");
                    }
                }else if(str_in_array(t.code, arr3, sizeof(arr3)/sizeof(arr3[0]))){
                    S();
                }else{
                    parse_error("if语句的语法不正确，使用了错误的符号");
                }
                EF(), ES();
                match = 1;
            }
        }

    }
    if(0 == match){
        parse_error("while/for/if/function/switch语句的语法不正确，使用了错误的符号");
    }
    if(!match_brackets("}")){
        // parse_error("CS");
    }
}

void PS(){
    if(!match_brackets("{")){
        parse_error("非法的语句开始符号");
    }
    // PS -> (br | ct | ec RV| rt RV | AS | RV);
    // first(PS) = {br , ct , ec , rt , id , ! , ++ , -- , s , i , ui , f , uf , fs}
    // first(RV) = {id , ! , ++ , -- , s , i , ui , f , uf , fs}
    // first(AE) = {id}
    string arr1[] = {"!", "++", "--", _S, I, UI, F, UF, FS};
    string arr2[] = {"," , "="};
    if(BR == t.code){
        forward_word();
    }else if(CT == t.code){
        forward_word();
    }else if(EC == t.code){
        forward_word();
        RV();
    }else if(RT == t.code){
        forward_word();
        RV();
    }else if(ID == t.code){
        // AS RV 冲突
        code_val cv1 = t;
        forward_word();
        qc.push(cv1);
        qc.push(t);
        if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
            forward_word();
            AS();
        }else{
            forward_word();
            RV();
        }
    }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        RV();
    }else{
        parse_error("错误的语句开头");
    }
    if(";" != t.code){
        parse_error("语句缺少;符号");
    }
    forward_word();
    if(!match_brackets("}")){
        // parse_error("PS");
    }
}

void SS(){
    if(!match_brackets("{")){
        parse_error("非法的语句开始符号");
    }
    // SS -> (((cs RV) | df) : B SS) | ε
    // first(SS) = {cs , df , ε}
    // follow(SS) = { '}' }
    int match = 0;
    if(_CS == t.code){
        forward_word();
        RV();
    }else if(DF == t.code){
        forward_word();
    }else{
        // 推出空字，判断是否∈follow(SS)
        if("}" == t.code){
            match = 1;
        }else{
            parse_error("switch语句缺少闭合的界符");
        }
    }
    if(0 == match){
        if(":" == t.code){
            forward_word();
            B(), SS();
            match = 1;
        }
    }
    if(0 == match){
        parse_error("case语句缺少:符号");
    }
    if(!match_brackets("}")){
        // parse_error("SS");
    }
}

void RV(){
    if(!match_brackets("(")){
        parse_error("非法的语句开始符号");
    }
    // RV -> VAR | MS | NM | STR | FC
    // first(VAR) = {id}
    // first(MS) = {id , ! , ++ , -- , s , i , ui , f , uf}
    // first(NM) = {i , ui , f , uf}
    // first(STR) = {s, id}
    // first(FC) = {fs}
    // 出现冲突
    string arr1[] = {"!", "++", "--"};
    string arr2[] = {I, UI, F, UF};
    string arr3[] = {")" , ";" , ":" , "]" , ","};
    code_val cv1 = t;
    if(FS == t.code){
        FC();
    }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        MS();
    }else if(_S == t.code){
        // STR MS冲突
        // forward_word();
        // qc.push(cv1);
        // qc.push(t);
        // if("." == t.code){
        //     forward_word();
        //     STR();
        // }else{
        //     forward_word();
        //     MS();
        // }
        STR();
    }else if(ID == t.code){
        // VAR MS STR冲突
        forward_word();
        qc.push(cv1);
        qc.push(t);
        if("." == t.code){
            forward_word();
            STR();
        }else if("[" == t.code || "=" == t.code){
            forward_word();
            VAR();
        }else{
            forward_word();
            MS();
        }
    }else if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
        // MS NM冲突
        forward_word();
        qc.push(cv1);
        qc.push(t);
        if(str_in_array(t.code, arr3, sizeof(arr3)/sizeof(arr3[0]))){
            forward_word();
            MS();
        }else{
            forward_word();
            NM();
        }
    }else{
        parse_error("右值语法错误");
    }
    if(!match_brackets(")")){
        // parse_error("RV");
    }
}

void AS(){
    if(!match_brackets("{")){
        parse_error("非法的语句开始符号");
    }
    // AS -> T1 (VAR | AE)
    // T1 -> ((VAR | AE) , T1) | ε
    // first(AS) = {id}
    // 冲突
    // string arr1[] = {")", ",", ";"};
    if(ID == t.code){
        do {
            code_val cv1 = t;
            forward_word();
            qc.push(cv1);
            qc.push(t);
            if ("=" == t.code) {
                forward_word();
                AE();
            } else {
                forward_word();
                VAR();
            }
        }while(("," == t.code) && forward_word() && (ID == t.code));
        // if(!str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        //     parse_error("AS");
        // }
    }
    if(!match_brackets("}")){
        // parse_error("AS");
    }
}

void MS(){
    if(!match_brackets("(")){
        parse_error("非法的语句开始符号");
    }
    // MS -> T2 ME
    // T2 -> (ME MO T2) | ε
    // MO -> (+ - * / % == > >= < <= != && || )
    // first(MS) = {id , ! , ++ , -- , s , i , ui , f , uf}
    // follow(MS) = {) , ; , : , ] , ,(逗号)}
    string arr1[] = {ID, "!", "++", "--", _S, I, UI, F, UF};
    string arr2[] = {"+", "-", "*", "/", "%", "==", ">", ">=", "<", "<=", "!=", "&&", "||"};    //MO
    string arr3[] = {")", ";", ":", "]", ","};
    if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        ME();
        while(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
            MO(), ME();
        }
        if(!str_in_array(t.code, arr3, sizeof(arr3)/sizeof(arr3[0]))){
            parse_error("非法的算术表达式");
        }
    }else{
        parse_error("非法的算术表达式");
    }
    if(!match_brackets(")")){
        // parse_error("MS3");
    }
}

void EF(){
    // EF -> ((ef '(' (RV | AS) ')' ({B} | S) ) EF) | ε
    // first(EF) = {ef, ε}
    // first(RV) = {id , ! , ++ , -- , s , i , ui , f , uf , fs}
    // first(AS) = {id}
    // follow(EF) = {if , fn , wh , sw , fr , br , ct , ec , rt , id , es}
    string arr1[] = {"!" , "++" , "--" , _S , I , UI , F , UF , FS};
    string arr2[] = {"," , "="};
    string arr3[] = {IF, FN, WH, SW, FR, BR, CT, EC, RT, ID};
    string arr4[] = {IF, FN, WH, SW, FR, BR, CT, EC, RT, ID, _ES};
    if(_EF == t.code){
        forward_word();
        if("(" == t.code){
            forward_word();
            if(ID == t.code){
                // 冲突，用队列来解决
                code_val cv1 = t;
                forward_word();
                qc.push(cv1);
                qc.push(t);
                if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
                    forward_word();
                    AS();
                }else{
                    forward_word();
                    RV();
                }
            }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
                RV();
            }else{
                parse_error("EF");
            }
            if(")" == t.code){
                forward_word();
                if("{" == t.code){
                    forward_word();
                    B();
                    if("}" == t.code){
                        forward_word();
                    }else{
                        parse_error("EF");
                    }
                }else if(str_in_array(t.code, arr3, sizeof(arr3)/sizeof(arr3[0]))){
                    S();
                }else{
                    parse_error("EF");
                }
                EF();
            }
        }else{
            parse_error("EF");
        }
    }else if(!str_in_array(t.code, arr4, sizeof(arr4)/sizeof(arr4[0]))){
        parse_error("EF");
    }
}

void ES(){
    // ES -> (es ({B} | S)) | ε
    // first(ES) = {es, ε}
    // follow(ES) = {if , fn , wh , sw , fr , br , ct , ec , rt , id}
    string arr1[] = {IF, FN, WH, SW, FR, BR, CT, EC, RT, ID};   // ES 和 S
    if(_ES == t.code){
        forward_word();
        if("{" == t.code){
            forward_word();
            B();
            if("}" == t.code){
                forward_word();
            }else{
                parse_error("ES");
            }
        }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
            S();
        }else{
            parse_error("ES");
        }
    }else if(!str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        parse_error("ES");
    }
}

void AE(){
    if(!match_brackets("(")){
        parse_error("AE");
    }
    // AE -> VAR = RV
    // first(AE) = {id}
    int match = 0;
    if(ID == t.code){
        VAR();
        if("=" == t.code){
            forward_word();
            RV();
            match = 1;
        }
    }
    if(0 == match){
        parse_error("AE");
    }
    if(!match_brackets(")")){
        // parse_error("AE");
    }
}

void VAR(){
    if(!match_brackets("(")){
        parse_error("VAR");
    }
    // VAR -> id IDX
    // first(VAR) = {id}
    if(ID == t.code){
        forward_word();
        IDX();
    }else{
        parse_error("VAR");
    }
    if(!match_brackets(")")){
        // parse_error("VAR");
    }
}

void NM(){
    if(!match_brackets("(")){
        parse_error("NM");
    }
    // NM -> i | ui | f | uf
    string arr1[] = {I, UI, F, UF};
    if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        forward_word();
    }else{
        parse_error("NM");
    }
    if(!match_brackets(")")){
        // parse_error("NM");
    }
}

void STR(){
    if(!match_brackets("(")){
        parse_error("STR");
    }
    // STR -> T3 (s | VAR)
    // T3 -> ((s | VAR) . T3) | ε
    // first(STR) = {s,id}
    do {
        if (_S == t.code) {
            forward_word();
        } else if (ID == t.code) {
            VAR();
        } else {
            parse_error("STR1");
        }
    }while("." == t.code && forward_word());
    if(!match_brackets(")")){
        // parse_error("STR2");
    }
}

void FC(){
    if(!match_brackets("(")){
        parse_error("FC");
    }
    // FC -> fs '(' CP ')'
    int match = 0;
    if(FS == t.code){
        forward_word();
        if("(" == t.code){
            forward_word();
            CP();
            if(")" == t.code){
                forward_word();
                match = 1;
            }
        }
    }
    if(0 == match){
        parse_error("FC");
    }
    if(!match_brackets(")")){
        // parse_error("FC");
    }
}

void ME(){
    if(!match_brackets("(")){
        parse_error("ME");
    }
    // ME -> ((! | ++ | --) VAR) | (VAR (++ | --) | VAR | NM | STR
    // first(ME) = {! , ++ , -- , id , s , i , ui , f , uf}
    // first(VAR) = {id}
    // first(NM) = {i , ui , f , uf}
    // first(STR) = {s,id}
    string arr1[] = {"!", "++", "--"};
    string arr2[] = {I, UI, F, UF};
    if(_S == t.code){
        STR();
    }else if(ID == t.code){
        // 冲突 VAR STR
        code_val cv1 = t;
        forward_word();
        qc.push(cv1);
        qc.push(t);
        if("[" == t.code || "=" == t.code){
            // VAR
            forward_word();
            VAR();
            if("++" == t.code || "--" == t.code){
                forward_word();
            }
        }else{
            forward_word();
            STR();
        }
    }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        forward_word();
        VAR();
    }else if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
        NM();
    }else{
        parse_error("ME");
    }
    if(!match_brackets(")")){
        // parse_error("ME");
    }
}

void MO(){
    // MO -> (+ - * / % == > >= < <= != && || )
    string arr1[] = {"+", "-", "*", "/", "%", "==", ">", ">=", "<", "<=", "!=", "&&", "||"};    //MO
    if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        forward_word();
    }else{
        parse_error("MO");
    }
}

void IDX(){
    // IDX -> ([ RV ] IDX) | ε
    // follow(IDX) = {= , ++ , -- , .(字符分隔符) + - * / % == > >= < <= != && || ) ; : ] ,(逗号)}
    string arr1[] = {"=", "++", "--", ".", "+", "-", "*", "/", "%", "==", ">", ">=", "<", "<=", "!=", "&&", "||", ")", ";", ":", "]", ","};
    if("[" == t.code){
        forward_word();
        RV();
        if("]" == t.code){
            forward_word();
            IDX();
        }else{
            parse_error("IDX");
        }
    }else if(!str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        parse_error("IDX");
    }
}

void CP(){
    // CP 本身可以为空字，则)是它的follow集元素
    // CP -> T4 RV
    // T4 -> ((RV ,) T4) | ε
    // first(CP) = {id , ! , ++ , -- , s , i , ui , f , uf , fs}
    string arr1[] = {ID, "!", "++", "--", _S, I, UI, F, UF, FS};
    if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        RV();
        while("," == t.code){
            forward_word();
            RV();
        }
    }else if(")" != t.code){
        parse_error("CP");
    }
}




