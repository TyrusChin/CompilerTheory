<?

	function __construct($store_method,$domain = ''){
		switch($store_method){
			case 'EmeSaeStorage':
				return EmeSaeStorage($domain);
			case 'EmeOSS':
				return EmeOSS($domain);
			case 'EmeIOOperation':
				return EmeIOOperation();
			default:
				return 0;
		}
	}



	function __construct($domain){
		$domain = $domain;
		$storage = SaeStorage();
	}

	function upload($srcFileName,$destFileName){
		$attr = array('gzip');
		return upload($domain,$destFileName, $srcFileName, $attr, 1);
	}

	function write($content,$file_path){
		return write($domain,$file_path,$content);
	}

	function upload($srcFileName,$destFileName){
		if(move_uploaded_file($srcFileName,$destFileName)){
			return $destFileName;
		}else{
			return 0;
		}
	}

	function write($content,$file_path){
		$file = fopen($file_path,"w");
		fwrite($file,$content);
		fclose($file);
		return $file_path;
	}

?>
