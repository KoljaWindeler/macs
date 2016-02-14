<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);

session_start();
require_once("../helper.php");
//require_once("sec.php");
require_once("../con.php");
require_once("../data.php");

$DEBUG=0;
$resume=0;
//////////////// LOG /////////////////
$sql_limit="LIMIT 0,50";
if(isset($_GET["logall"]) or (isset($_SESSION["sql_limit"]) && $_SESSION["sql_limit"]=="none")){
	$sql_limit="";
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
$log_sort_column="id";
$log_sort_dir=$link[0][1];

$MACRO_LOG_SORT_COLUMN="LOG_SORT_COLUMN";
$MACRO_LOG_SORT_DIR="LOG_SORT_DIR";


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
	$link[$i][3]='<a href="index.php?'.$MACRO_LOG_SORT_COLUMN.'='.$link[$i][0].'&'.$MACRO_LOG_SORT_DIR.'='.$link[$i][1].'#log">'.$link[$i][2].'</a>';
};

$o_log="";
$stmt = $db->prepare('SELECT * FROM log ORDER BY `'.$log_sort_column.'` '.$log_sort_dir.' '.$sql_limit);
$stmt->execute();
foreach ($stmt as $row) {
	$u_out="-";
	$m_out="-";
	$l_out="-";
	$extra="-";

	if($row["machine_id"]>0){
		$m=get_mach($row["machine_id"]);
		if($m!=-1){
			$m_out=$m["name"];
		} else {
			$m_out="db hickup";
		};
	}

	if($row["user_id"]>0){
		$u=get_user($row["user_id"]);
		if($u!=-1){
			$u_out=$u["name"];
		} else {
			$u_out="db hickup";
		};
	};

		// extra info

	if($row["login_id"]>0){
		$l=get_user($row["login_id"]);
		if($l!=-1){
			$extra='by '.$l["name"];
		} else {
			$extra="by "."db hickup";
		};
	} else if($row["usage"]!="-" && !empty($row["usage"]) && $extra=="-"){
		$d[0]=floor($row["usage"]/3600);
		$d[1]=floor(($row["usage"]%3600)/60);
		$d[2]=$row["usage"]%60;
		for($i=0; $i<3; $i++){
			$ii=0;
			while(strlen($d[$i])<2 && $ii<5){
				$d[$i]="0".$d[$i];
				$ii++;
			}
		}
		$extra="Usage ".$d[0].":".$d[1].":".$d[2];
	};
	
	$row["event"]=str_replace(";",",",$row["event"]);
	$m_out=str_replace(";",",",$m_out);
	$u_out=str_replace(";",",",$u_out);
	$extra=str_replace(";",",",$extra);

 	$o_log.=date("Y/m/d H:i",$row["timestamp"]).';'.$row["event"].';'.$m_out.';'.$u_out.';'.$extra."\r\n";

};
//////////////// LOG /////////////////

echo $o_log;

?>
