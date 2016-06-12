<?

    
    for($i = 0; $i < 10; $i ++){
        if($i == 4) continue;
        echo $i;
    }

    
    $num = 6;
    for(;;){
        $array[$num] = $num;
        if(--$num) break;
    }

    
    for($i = 0; $i < 5 && 1 == 1; $i++){
        for($j = 0; $j <= $i; $j++){
            $_2d_array[("num".$i)][$j] = $i * $j;
        }
    }

    
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

    
    $sum = hello(3, "bye bye") + $a;
    $string = $sum."string cat";

    
    $abc = (5 + 7) * 3 ;

?>
#