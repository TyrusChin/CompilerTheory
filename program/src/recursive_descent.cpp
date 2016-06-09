#include <fstream>
#include <iostream>
#include "../header/word_code.h"
#include <queue>
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

typedef struct cvNode {
    struct code_val node;
    struct cvNode* next;
    struct cvNode* front;
} cvNode, *pCvNode;

int forward_word(); // 声明
char tval[1024] = {'\0'};

// 全局的单词结构，指向当前的处理单词
code_val t;
pCvNode pn; // 当前结点指针
ifstream cinf;

// 错误输出函数 debug
//void parse_error(string func, string code = t.code){
//    cout << endl << "Error in " << func << "->" << code << endl;
//    exit(0);
//}

// 错误输出函数 正式的
void parse_error(string func, string reason, string code = t.code){
    cout << endl << func << "函数中，" << reason << " : " << code << endl;
    exit(0);
}

// 主函数
void parser(char code_val_file[]){
    cinf.open(code_val_file, ios::in);
    cvNode* const start = new cvNode();
    pCvNode p = start;  // 头节点
    p -> next = 0;
    p -> front = 0;
    while(cinf >> t.code){
        cinf.getline(tval, sizeof(tval));
        t.val = tval;
        pCvNode q = new cvNode();
        q -> node = t;
        q -> front = p;
        q -> next = 0;
        p -> next = q;
        p = q;
    }
    pn = start;
    forward_word();
    P();
    if(t.code == "#"){
        cout << endl << "语法正确" << endl;
    }else{
        parse_error(__FUNCTION__, "多余的符号");
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
    pn = pn -> next;
    t = pn -> node;
    cout << t.code << " " << t.val << endl;
    return 1;
}

int backward_word(){
    // 后退一个单词
    pn = pn -> front;
    t = pn -> node;
    return 1;
}

// 匹配括号
// 调用时机：以下的情况所对应的函数，如STR()，函数体内第一句和最后一句
// 支持大括号的位置：B CS PS SS AS
// 支持小括号的位置：RV NM ME STR VAR FC MS AE
int match_brackets(string symbol, int match_time = -1){
    // 传入{}() 的其中一种，返回symbol的个数。
    // match_time 返回匹配的次数，一般传入该非终结符的{(的匹配数，以防影响到本来的文法)}匹配，-1表示全匹配
    int count = 0;
    int matched = 0;
    while (match_time != 0 && symbol == t.code) {
        ++ count;
        forward_word();
        if(-1 != match_time){
            ++ matched;
            if(match_time == matched){
                break;
            }
        }
    }
    return count;
}

// 处理产生式头部的符号，如 {(等
void enqueue_brackets(string symbol, queue<code_val> &tempq){
    // symbol = { or (
    while(symbol == t.code){
        code_val cv1 = t;
        forward_word();
        tempq.push(cv1);
    }
}

void dequeue_brackets(queue<code_val> &tempq){
    while(!tempq.empty()){
        backward_word();
        tempq.pop();
    }

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
        parse_error(__FUNCTION__, "缺少<?开始符号，或者非法的结束符");
    }
    // 如果是#的情况，则处理成空字的情况
}

void B(){
    // B -> (S B) | ε
    // first(B) = {if , fn , wh , sw , fr , br , ct , ec , rt , id, ! , ++ , -- , s , i , ui , f , uf , fs , ε}
    // follow(B) = {ed , } , cs , df}
    string arr1[] = {IF, FN, WH, SW, FR, BR, CT, EC, RT, ID, "!", "++", "--", _S, I, UI, F, UF, FS};
    string arr2[] = {ED, _CS, DF, "}"};
    if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        S(), B();
    }else if(!str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
        parse_error(__FUNCTION__, "非法的语句开始符号");
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
    queue<code_val> tempq ;
    enqueue_brackets("{", tempq);
    if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        dequeue_brackets(tempq);
        CS();
    }else if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
        dequeue_brackets(tempq);
        PS();
    }else{
        parse_error(__FUNCTION__, "非法的语句开始符号");
    }
}

void CS(){
    const int bcount = match_brackets("{");
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
                forward_word();
                if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
                    backward_word();
                    AS();
                }else{
                    backward_word();
                    RV();
                }
            }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
                RV();
            }else{
                parse_error(__FUNCTION__, "while语句语法不正确");
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
            queue<code_val> tempq ;
            enqueue_brackets("(", tempq);
            if(ID == t.code){
                // 冲突，用队列来解决
                forward_word();
                if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
                    dequeue_brackets(tempq);
                    backward_word();
                    AS();
                }else{
                    dequeue_brackets(tempq);
                    backward_word();
                    RV();
                }
            }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
                dequeue_brackets(tempq);
                RV();
            }else{
                parse_error(__FUNCTION__, "if语句的条件判断语法不正确");
            }
            if(")" == t.code) {
                forward_word();
                if("{" == t.code){
                    forward_word();
                    B();
                    if("}" == t.code){
                        forward_word();
                    }else{
                        parse_error(__FUNCTION__, "if语句的语法不正确，可能缺少一些必要的界符");
                    }
                }else if(str_in_array(t.code, arr3, sizeof(arr3)/sizeof(arr3[0]))){
                    S();
                }else{
                    parse_error(__FUNCTION__, "if语句的语法不正确，使用了错误的符号");
                }
                EF(), ES();
                match = 1;
            }
        }

    }
    if(0 == match){
        parse_error(__FUNCTION__, "while/for/if/function/switch语句的语法不正确，使用了错误的符号");
    }
    const int ecount = match_brackets("}", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "大括号不匹配");
    }
}

void PS(){
    const int bcount = match_brackets("{");
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
        forward_word();
        if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
            backward_word();
            AS();
        }else{
            backward_word();
            RV();
        }
    }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        RV();
    }else{
        parse_error(__FUNCTION__, "错误的语句开头");
    }
    if(";" != t.code){
        parse_error(__FUNCTION__, "语句缺少;符号");
    }
    forward_word();
    const int ecount = match_brackets("}", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "大括号不匹配");
    }
}

void SS(){
    const int bcount = match_brackets("{");
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
            parse_error(__FUNCTION__, "switch语句缺少闭合的界符");
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
        parse_error(__FUNCTION__, "case语句缺少:符号");
    }
    const int ecount = match_brackets("}", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "大括号不匹配");
    }
}

void RV(){
    const int bcount = match_brackets("(");
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
    queue<code_val> tempq ;
    enqueue_brackets("(", tempq);
    if(FS == t.code){
        dequeue_brackets(tempq);
        FC();
    }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        dequeue_brackets(tempq);
        MS();
    }else if(_S == t.code){
        // STR MS冲突
        // forward_word();
        // if("." == t.code){
        //     backward_word();
        //     STR();
        // }else{
        //     backward_word();
        //     MS();
        // }
        dequeue_brackets(tempq);
        STR();
    }else if(ID == t.code){
        // VAR MS STR冲突
        forward_word();
        if("." == t.code){
            backward_word();
            dequeue_brackets(tempq);
            STR();
        }else if("=" == t.code){
            backward_word();
            dequeue_brackets(tempq);
            VAR();
        }else{
            queue<code_val> tempq1;
            while("[" == t.code){
                tempq1.push(t);
                forward_word();
                while(true){
                    tempq1.push(t);
                    if("]" == t.code){
                        forward_word();
                        break;
                    }
                    forward_word();
                }
            }
            tempq1.push(t);
            if("=" == t.code){
                dequeue_brackets(tempq1);
                VAR();
            }else{
                dequeue_brackets(tempq1);
                MS();
            }
        }
    }else if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
        // MS NM冲突
        forward_word();
        dequeue_brackets(tempq);
        if(str_in_array(t.code, arr3, sizeof(arr3)/sizeof(arr3[0]))){
            backward_word();
            NM();
        }else{
            backward_word();
            MS();
        }
    }else{
        parse_error(__FUNCTION__, "右值语法错误");
    }
    const int ecount = match_brackets(")", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "小括号不匹配");
    }
}

void AS(){
    const int bcount = match_brackets("{");
    // AS -> T1 (VAR | AE)
    // T1 -> ((VAR | AE) , T1) | ε
    // first(AS) = {id}
    // 冲突
    // string arr1[] = {")", ",", ";"};
    if(ID == t.code){
        do {
            forward_word();
            if ("=" == t.code) {
                backward_word();
                AE();
            } else {
                backward_word();
                VAR();
            }
        }while(("," == t.code) && forward_word() && (ID == t.code));
        // if(!str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        //     parse_error(__FUNCTION__, "AS");
        // }
    }
    const int ecount = match_brackets("}", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "大括号不匹配");
    }
}

void MS(){
    const int bcount = match_brackets("(");
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
            parse_error(__FUNCTION__, "非法的算术表达式1");
        }
    }else{
        parse_error(__FUNCTION__, "非法的算术表达式2");
    }
    const int ecount = match_brackets(")", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "小括号不匹配");
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
    string arr4[] = {IF, FN, WH, SW, FR, BR, CT, EC, RT, ID, _ES, "}"};
    if(_EF == t.code){
        forward_word();
        if("(" == t.code){
            forward_word();
            if(ID == t.code){
                forward_word();
                if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
                    backward_word();
                    AS();
                }else{
                    backward_word();
                    RV();
                }
            }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
                RV();
            }else{
                parse_error(__FUNCTION__, "elseif条件语法错误");
            }
            if(")" == t.code){
                forward_word();
                if("{" == t.code){
                    forward_word();
                    B();
                    if("}" == t.code){
                        forward_word();
                    }else{
                        parse_error(__FUNCTION__, "elseif语句缺少闭合的界符");
                    }
                }else if(str_in_array(t.code, arr3, sizeof(arr3)/sizeof(arr3[0]))){
                    S();
                }else{
                    parse_error(__FUNCTION__, "elseif语句语法错误");
                }
                EF();
            }
        }else{
            parse_error(__FUNCTION__, "elseif语句缺少闭合的界符");
        }
    }else if(!str_in_array(t.code, arr4, sizeof(arr4)/sizeof(arr4[0]))){
        parse_error(__FUNCTION__, "elseif语句附近出现非法字符");
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
                parse_error(__FUNCTION__, "else语句缺少闭合的界符");
            }
        }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
            S();
        }else{
            parse_error(__FUNCTION__, "else语句语法错误");
        }
    }else if(!str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0])) && "}" != t.code){
        parse_error(__FUNCTION__, "else语句附近出现非法字符");
    }
}

void AE(){
    const int bcount = match_brackets("(");
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
        parse_error(__FUNCTION__, "赋值语句语法错误");
    }
    const int ecount = match_brackets(")", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "小括号不匹配");
    }
}

void VAR(){
    const int bcount = match_brackets("(");
    // VAR -> id IDX
    // first(VAR) = {id}
    if(ID == t.code){
        forward_word();
        IDX();
    }else{
        parse_error(__FUNCTION__, "变量定义语法错误");
    }
    const int ecount = match_brackets(")", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "小括号不匹配");
    }
}

void NM(){
    const int bcount = match_brackets("(");
    // NM -> i | ui | f | uf
    string arr1[] = {I, UI, F, UF};
    if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        forward_word();
    }else{
        parse_error(__FUNCTION__, "不合法的数字表示");
    }
    const int ecount = match_brackets(")", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "小括号不匹配");
    }
}

void STR(){
    const int bcount = match_brackets("(");
    // STR -> T3 (s | VAR)
    // T3 -> ((s | VAR) . T3) | ε
    // first(STR) = {s,id}
    do {
        if (_S == t.code) {
            forward_word();
        } else if (ID == t.code) {
            VAR();
        } else {
            parse_error(__FUNCTION__, "字符串定义不合法");
        }
    }while("." == t.code && forward_word());
    const int ecount = match_brackets(")", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "小括号不匹配");
    }
}

void FC(){
    const int bcount = match_brackets("(");
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
        parse_error(__FUNCTION__, "函数调用语法不合法");
    }
    const int ecount = match_brackets(")", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "小括号不匹配");
    }
}

void ME(){
    const int bcount = match_brackets("(");
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
        forward_word();
        if("[" == t.code || "=" == t.code){
            // VAR
            backward_word();
            VAR();
            if("++" == t.code || "--" == t.code){
                forward_word();
            }
        }else{
            backward_word();
            STR();
        }
    }else if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        forward_word();
        VAR();
    }else if(str_in_array(t.code, arr2, sizeof(arr2)/sizeof(arr2[0]))){
        NM();
    }else{
        parse_error(__FUNCTION__, "不合法的算术表达式");
    }
    const int ecount = match_brackets(")", bcount);
    if(bcount != ecount){
        parse_error(__FUNCTION__, "小括号不匹配");
    }
}

void MO(){
    // MO -> (+ - * / % == > >= < <= != && || )
    string arr1[] = {"+", "-", "*", "/", "%", "==", ">", ">=", "<", "<=", "!=", "&&", "||"};    //MO
    if(str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        forward_word();
    }else{
        parse_error(__FUNCTION__, "不支持的运算符");
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
            parse_error(__FUNCTION__, "数组缺少闭合界符]");
        }
    }else if(!str_in_array(t.code, arr1, sizeof(arr1)/sizeof(arr1[0]))){
        parse_error(__FUNCTION__, "非法操作数组");
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
        parse_error(__FUNCTION__, "函数调用的参数列表缺少闭合界符)");
    }
}




