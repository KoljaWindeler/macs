<?php

function set_mach_outdated($mach_id,$db){
	$stmt2 = $db->prepare("INSERT INTO  `macs`.`update_available` (`mach_id`) VALUE (:mach_id)");
        $stmt2->bindParam(":mach_id",$mach_id,PDO::PARAM_INT);
        $stmt2->execute();
};

function hide_table($name){
	global $_GET;
	$show="hideatstart";
	if(isset($_GET["show"])){
	        if($_GET["show"]==$name){ // TODO ARRAY
        	        $show="";
	        }
	};
	return $show;
};

function show_info($txt){
	global $info;
	global $info_txt;
	if(!empty($info_txt)){
		$info_txt.=",".$txt;
	} else {
		$info_txt=$txt;
	};

	$info='<div id="info" class="info">'.$info_txt.'</div>';
};



?>
