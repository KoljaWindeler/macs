<?php
load_data();

function load_data(){
	global $db_user;
	global $db_mach;
	global $db;

	// user
	$db_user="";
	$stmt = $db->prepare('SELECT * FROM user');
	$stmt->execute();
	foreach ($stmt as $row) {
		$db_user[$row["id"]]=$row;
	};

	// mach
	$db_mach="";
	$stmt = $db->prepare('SELECT * FROM mach');
	$stmt->execute();
	foreach ($stmt as $row) {
		$db_mach[$row["id"]]=$row;
	};
};


function get_user($i){
	global $db_user;
	if($db_user[$i]!=null){
		return $db_user[$i];
	};
	return -1;
}

function get_user_name($i){
	$u=get_user($i);
	if($u!=-1){
		return $u["name"];
	};
	return -1;
};

function get_mach($i){
	global $db_mach;
	if($db_mach[$i]!=null){
		return $db_mach[$i];
	};
	return -1;
}

function get_mach_name($i){
	$u=get_mach($i);
	if($u!=-1){
		return $u["name"];
	};
	return -1;
};

?>
