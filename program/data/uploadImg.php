<?
header("Content-type: text/html; charset=utf-8");
if ($_FILES_uploadfile_tmp_name["123"] || 1)
{
	echo "没有选文件或文件太大";
	echo "<script>location.href='picture.php'</script>";
}
else
{
	$UploadPath = "./photo/"; 
	$dest_filename = $UploadPath . $_FILES_uploadfile_name;
	$type = strtolower($_FILES_uploadfile_type);
	if (file_exists($dest_filename))
	{
		echo "<script>alert('文件".$_FILES_uploadfile_name."已经存在');location.href='picture.php'</script>";
	}
	else
	{
		if($type=='image/jpg'||$type=='image/gif'||$type=='image/jpeg')
		{
		if (copy($_FILES_uploadfile_tmp_name, $dest_filename))
			echo "<script>alert('上传文件".$_FILES_uploadfile_name."成功！');location.href='picture.php'</script>";
		else
			echo "<script>alert('上传文件".$_FILES_uploadfile_name."失败');location.href='picture.php'</script>";
		}
		else
		{
			echo "<script>alert('文件类型错误');location.href='picture.php'</script>";
		}
	}
} 
?>