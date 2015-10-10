<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
date_default_timezone_set("UTC");

//////////////// LOG /////////////////
$title="(50 most recent entries - <a href='".$_SERVER["SCRIPT_NAME"]."?logall#log'>show all</a>)";
$sql_limit="LIMIT 0,50";
if(isset($_GET["loglimited"])){
	$_SESSION["sql_limit"]="yes";
} elseif(isset($_GET["logall"]) or (isset($_SESSION["sql_limit"]) && $_SESSION["sql_limit"]=="none")){
	$title="(all) - <a href='".$_SERVER["SCRIPT_NAME"]."?loglimited#log'>show only 50 entries</a>";
	$sql_limit="";
	$_SESSION["sql_limit"]="none";
};

// prepare new links
$link[0][0]="timestamp";
$link[0][1]="desc";
$link[0][2]="Time";

$link[1][0]="event";
$link[1][1]="desc";
$link[1][2]="Event";

$link[2][0]="machine_id";
$link[2][1]="desc";
$link[2][2]="Machine";

$link[3][0]="user_id";
$link[3][1]="desc";
$link[3][2]="User";

//// sorting
$log_sort_column=$link[0][0];
$log_sort_dir=$link[0][1];

$MACRO_LOG_SORT_COLUMN="LOG_SORT_COLUMN";
$MACRO_LOG_SORT_DIR="LOG_SORT_DIR";


// accept incoming parameter
if(isset($_GET[$MACRO_LOG_SORT_COLUMN])){
	for($i=0;$i<count($link);$i++){
		if($_GET[$MACRO_LOG_SORT_COLUMN]==$link[$i][0]){
			$_SESSION[$MACRO_LOG_SORT_COLUMN]=$_GET[$MACRO_LOG_SORT_COLUMN];
		};
	};
};

if(isset($_GET[$MACRO_LOG_SORT_DIR])){
	if($_GET[$MACRO_LOG_SORT_DIR]=="asc" || $_GET[$MACRO_LOG_SORT_DIR]=="desc"){
		$_SESSION[$MACRO_LOG_SORT_DIR]=$_GET[$MACRO_LOG_SORT_DIR];
	}
};

if(isset($_SESSION[$MACRO_LOG_SORT_COLUMN])){
	$log_sort_column=$_SESSION[$MACRO_LOG_SORT_COLUMN];
};
if(isset($_SESSION[$MACRO_LOG_SORT_DIR])){
	$log_sort_dir=$_SESSION[$MACRO_LOG_SORT_DIR];
};


for($i=0; $i<count($link); $i++){
	if($link[$i][0]==$log_sort_column){
		if($link[$i][1]==$log_sort_dir){
			$link[$i][1]="asc";
		};
	};
	$link[$i][3]='<a href="'.$_SERVER["SCRIPT_NAME"].'?'.$MACRO_LOG_SORT_COLUMN.'='.$link[$i][0].'&'.$MACRO_LOG_SORT_DIR.'='.$link[$i][1].'#log">'.$link[$i][2].'</a>';
};

$o_log='<table class="fillme" id="logtable"><tr class="subheader"><td>'.$link[0][3].'</td><td>'.$link[1][3].'</td><td>'.$link[2][3].'</td><td>'.$link[3][3].'</td><td>Info</td></tr>';
$o_log.='<tr><td class="logentry" colspan="5">loading</td></tr>';

//////////////// LOG /////////////////

$o_log.='</td></tr></table>';

?>
