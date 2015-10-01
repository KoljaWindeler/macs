<?php
require_once('con.php');
if(isset($_GET["logme"]) && isset($_GET["event"]) && (isset($_GET["machine"]) || isset($_GET["user"]))){
	if(!isset($_GET["machine"])){
		$mach="-";
	} else {
		$mach=$_GET["machine"];
	}
	if(!isset($_GET["user"])){
		$user="-";
	} else {
		$user=$_GET["user"];
	}
	add_log($mach,$user,$_GET["event"]);
};


function add_log($machine,$user,$event){
	global $db;
	//echo "adding ".$machine." ".$user." ".$event;
	$stmt = $db->prepare("INSERT INTO  `macs`.`log` (`timestamp`,`user_id`,`machine_id`,`event`) VALUE (".time().",:user,:machine,:event)");
	$stmt->bindParam(":user",$user,PDO::PARAM_STR);
	$stmt->bindParam(":machine",$machine,PDO::PARAM_STR);
	$stmt->bindParam(":event",$event,PDO::PARAM_STR);
	$stmt->execute();
};

?>
