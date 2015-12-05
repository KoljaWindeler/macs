<?php
include_once("con.php");
if(isset($_GET["logout"])){
	$_SESSION['ID']=0;
	$_SESSION['bg']="";
	show_info("bye bye");
};

$login=0;
//var_dump($_POST);
if(!isset($_SESSION['ID']) or $_SESSION['ID']==0){
	if(isset($_POST["macs_login"]) and isset($_POST["macs_pw_md5"]) and !empty($_POST["macs_login"]) and !empty($_POST["macs_pw_md5"])){
		$stmt = $db->prepare("SELECT `hash`,`id` FROM `macs`.`user` where login=:login and active=1");
	        $stmt->bindParam(":login",$_POST["macs_login"],PDO::PARAM_STR);
        	$stmt->execute();

		foreach($stmt as $row){
			//echo "<br>given ".$_POST["macs_pw_md5"]." vs ".$row["hash"];
			if($row["hash"]==$_POST["macs_pw_md5"]){
				//echo "i guess you are in";
				$_SESSION['ID']=$row["id"];
				$login=1;
				//show_info("Good to see you again, ".$_POST["macs_login"]);
				echo "<script>window.location.replace('index.php?msg=Good to see you again, ".$_POST["macs_login"]."');</script>";
				break;
			}
		};
	}

	if(!$login){
		include_once("html.php");
		$o='<form action="index.php?" method="post" id="login_form">';
		$o.='<table class="logintable">';
		$o.='<tr><td><input type="text" id="macs_login" name="macs_login"></td><td><input type="password" id="macs_pw" name="macs_pw"></td></tr>';
		$o.='<tr><td colspan="2"><input type="hidden" name="macs_pw_md5" id="macs_pw_md5"><input type="submit"></td></tr></table></form>';
		echo $header.$o.$footer;
		exit(0);
	};
} else if(isset($_GET['msg'])) {
	show_info($_GET['msg']);
};
?>
