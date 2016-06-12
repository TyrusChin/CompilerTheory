<?
    // 采用简明的语法 <? ?>，没有php标记

    $a = "string123";
    // 这里是一个中文的行注释
    /*
        this is a block notes
        lalala

        !!
    */

    $x = 1 + 2\\
    ;   // 续行符
    $conn_str = '单引号的'.$a."双引号的";
    $double = "\n\t";       $single = '\n\t';

    // 变态的测试
    $what = '\' 假装是一个字符串结束符 //假装是一个注释！';
    $what2 = "\" /*?????? block notes??*/ ";
?>

