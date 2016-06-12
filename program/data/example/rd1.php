<?

    // for语句、赋值语句、判断条件、自增、if语句、continue、输出
    for($i = 0; $i < 10; $i ++){
        if($i == 4) continue;
        echo $i;
    }

    // 赋值、缺省条件的for、数组、没有大括号的if语句、break
    $num = 6;
    for(;;){
        $array[$num] = $num;
        if(--$num) break;
    }

    // 嵌套for、多条件、二维关联数组、字符串拼接
    for($i = 0; $i < 5 && 1 == 1; $i++){
        for($j = 0; $j <= $i; $j++){
            $_2d_array[("num".$i)][$j] = $i * $j;
        }
    }

    // while语句、if-elseif-else语句、函数调用
    $a = 0;
    while(1){
        if($a == 1){
            break;
        }elseif(isset($b)){
            echo 123;
        }else{
            $a++;
        }
    }

    // 定义函数、参数组(参数初始化)、switch语句、case-default、return语句、浮点数、有符号浮点数、有符号整数
    function hello($arg1, $arg2 = "hello"){
        switch($arg2){
            case "hello":
                echo "world";
                $a = 1.5;
                break;
            case "bad":
                echo "no!";
                $a = +3;
                break;
            default :
                echo "damn";
                $a = -2.5;
        }
        return ($arg1 / $a);
    }

    // 函数调用的赋值、字符串拼接
    $sum = hello(3, "bye bye") + $a;
    $string = $sum."string cat";

?>
