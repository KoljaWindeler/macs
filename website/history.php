<?php

ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);


require_once('con.php');
if(isset($_GET["logme"]) && isset($_GET["event"]) && (isset($_GET["mach_nr"]) || isset($_GET["badge"]))){
	$event=$_GET["event"];

	if(isset($_GET["timeopen"])){
		$time=$_GET["timeopen"];
	} else {
		$time="-";
	};

	if(!isset($_GET["mach_nr"])){
		$mach="-";
	} else {
		$stmt = $db->prepare("SELECT `id` FROM  `macs`.`mach` WHERE mach_nr=:mach_nr");
		$stmt->bindParam("mach_nr",$_GET["mach_nr"],PDO::PARAM_STR);
		$stmt->execute();
		foreach($stmt as $row){
			$mach=$row["id"];
		};
	}
	if(!isset($_GET["badge"])){
		$user="-";
	} else {
		$user="";
		$stmt = $db->prepare("SELECT `id` FROM  `macs`.`user` WHERE badge_id=:badge and active=1");
		$stmt->bindParam("badge",$_GET["badge"],PDO::PARAM_STR);
		$stmt->execute();
		foreach($stmt as $row){
			$user=$row["id"];
		};
		if($user==""){
			$event.=" ".$_GET["badge"];
			$user="-";
		};
	}
	add_log($mach,$user,$event,$time);
};

function add_log($machine,$user,$event,$time){
	global $db;
	//echo "adding ".$machine." ".$user." ".$event;
	$stmt = $db->prepare("INSERT INTO  `macs`.`log` (`timestamp`,`user_id`,`machine_id`,`event`,`login_id`,`usage`) VALUE (".time().",:user,:machine,:event,:login,:time)");
	$stmt->bindParam(":user",$user,PDO::PARAM_STR);
	$stmt->bindParam(":machine",$machine,PDO::PARAM_STR);
	$stmt->bindParam(":event",$event,PDO::PARAM_STR);
	$stmt->bindParam(":login",$_SESSION['ID'],PDO::PARAM_INT);
	$stmt->bindParam(":time",$time,PDO::PARAM_INT);
	$stmt->execute();
};

?>
