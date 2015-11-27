<?php
include_once('../con.php');
$db_wifi="";
$status="";
$o_setup="";
$o_setup_below="";

ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);

$header='<!DOCTYPE html><html><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="x-dns-prefetch-control" content="off">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta name="robots" content="noindex">
<title>MACS - Dashboard</title>
<link rel="stylesheet" href="../css/jquery-ui.css"/>
<link rel="stylesheet" href="../css/iframe.css"/>';

$o_setup.=$header;

//var_dump($_POST);


if(isset($_POST["save_wifi"])){
	$stmt = $db->prepare("TRUNCATE wifi");
	$stmt->execute();
	$all_ok=1;

	for($i=0;$i<3;$i++){
		$stmt = $db->prepare("INSERT INTO  `macs`.`wifi` (`id`,`ssid`,`pw`,`type`) VALUE (".$i.",:ssid,:pw,:type)");
		$stmt->bindParam(":ssid",$_POST["ssid_".$i],PDO::PARAM_STR);
		$stmt->bindParam(":pw",$_POST["pw_".$i],PDO::PARAM_STR);
		$stmt->bindParam(":type",$_POST["type_".$i],PDO::PARAM_STR);
		$stmt->execute();
		// save it but check if after wards to keep the input in the database
		if(strlen($_POST["ssid_".$i])>20){
			$msg="SSID length is limited to 20 chars";
			$all_ok=0;
		};
                if(strlen($_POST["pw_".$i])>20){
                        $msg="PW length is limited to 20 chars";
                        $all_ok=0;
                };

	};

	if($all_ok==1){
		$o_setup_below.='<iframe src="module_setup_helper_execute.php" width="0" height="0" border="0"></iframe>';
		$o_setup_below.='<script src="../js/jquery.min.js" type=\'text/javascript\'></script>';
		$o_setup_below.='<script src="../js/setup_helper.js" type=\'text/javascript\'></script>';
		$status="Loading";
	} else {
		$o_setup_below.=$msg;
	};
};

// grab all wifi from db
$stmt = $db->prepare('SELECT * FROM wifi');
$stmt->execute();
foreach ($stmt as $row) {
	if($row["id"]>=0 && $row["id"]<=2){
		$i=$row["id"];
		$db_wifi[$i]["ssid"]=$row["ssid"];
		$db_wifi[$i]["pw"]=$row["pw"];
		$db_wifi[$i]["type"]=$row["type"];
	}
};

$o_setup.='<form method="POST" action=""><table class="fillme" width="100%" height=""><tr class="subheader"><td>WiFi</td><td>SSID</td><td>password</td><td>type</td></tr>';

for($i=0; $i<3; $i++){
	if(empty($db_wifi[$i])){
		$db_wifi[$i]["ssid"]="";
		$db_wifi[$i]["pw"]="";
		$db_wifi[$i]["type"]="";
	}

	$title="";
	if($i==0){
		$title="MACS";
	} else if($i==1){
		$title="Primary update";
	}	else {
		$title="Backup update";
	};

	$o_setup.='<tr><td>'.$title.'</td>
		<td><input type="text" name="ssid_'.$i.'" value="'.$db_wifi[$i]["ssid"].'"></input></td>
		<td><input type="text" name="pw_'.$i.'" value="'.$db_wifi[$i]["pw"].'"></input></td>
		<td><select name="type_'.$i.'">';

	$type=array("","open","wep","wpa","wpa2");
	for($ii=1; $ii<count($type);$ii++){
		$o_setup.='<option value="'.$ii.'"';
		if($ii==$db_wifi[$i]["type"]){
			$o_setup.=' selected';
		};
		$o_setup.='>'.$type[$ii].'</option>';
	};

	$o_setup.='</td>';
};

$o_setup.='<tr><td colspan="4"><input type="submit" name="save_wifi" value="Go"></input></td></tr>';
$o_setup.='<tr><td colspan="4"><br><br><div id="setup_log">'.$status.'</div></td></tr>';
$o_setup.='</table></form>';

$o_setup.=$o_setup_below;
echo $o_setup;
?>
