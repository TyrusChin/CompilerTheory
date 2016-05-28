<?php
header("Content-type: text/html; charset=utf-8"); 
$file = $_GET['file'];

if(file_exists($file))
{
	if(unlink($file))
	{
		echo "<script>alert('删除文件", $file, "成功！');location.href='picture.php'</script>";
	}
	else
	{
		echo "<script>alert('删除文件", $file, "失败！');location.href='picture.php'</script>";
	}
	
}
else
{
	echo "<script>alert('文件", $file, "不存在！');location.href='picture.php'</script>";
}

?>