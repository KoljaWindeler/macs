<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
date_default_timezone_set("UTC");

//session_start();
require_once("helper.php");
//require_once("sec.php");
require_once("db_con.php");
require_once("data.php");


// step 1. - generate a config for all graphes within all charts
// step 2. - get all data, go as long as the machine id is the same, collect data
// step 3. - as soon as the machine-id changed run add_chart with the config and the collected data
//	     add that to the output

// cycle through all machines
function get_graph($machine_id){
	global $db;
	global $db_mach;
	global $db_user;
	// get all user that have been used the machine
	$user_on_machine=array();
	$req='SELECT distinct `user_id` from `log` where `machine_id`='.$machine_id.'';
	$stmt = $db->prepare($req);
	$stmt->execute();
	$i=0;
	foreach($stmt as $row){
		$user_on_machine[$i]["id"]=$row["user_id"];
		//failsafe
		if(!isset($db_user[$row["user_id"]])){
			$db_user[$row["user_id"]]["name"]="user not found";
		}
		$user_on_machine[$i]["title"]=$db_user[$row["user_id"]]["name"];
		$i++;
	};

	// now get all log entries, summed per day
	$req='SELECT sum(`usage`) as used_time, `user_id`, FLOOR(`timestamp`/86400)*86400 as date ';
	$req.='FROM `log` where `usage`>0 and `machine_id`='.$machine_id.' ';
	$req.='group by FLOOR(`timestamp`/86400), `user_id` ORDER BY `date` asc';

	//echo '<hr>'.$req.'<hr>';

	$stmt = $db->prepare($req);
	$stmt->execute();
	$collector=array();
	$log_entries=0;

	foreach ($stmt as $row) {
		$log_entries++;
		//echo "Time:".$row["used_time"]." user id=".$row["user_id"]." date=".$row["date"]." / ".date("Y-m-d",$row["date"])."<br>";

		// find the right index in the collector
		$found=-1;
		for($i=0;$i<count($collector);$i++){
			if(isset($collector[$i]["date"])){
				if($collector[$i]["date"]==$row["date"]){
					$found=$i;
					break;
				};
			};
		};

		if($found==-1){
			//echo "not found, adding entries<br>";
			$found=count($collector);
			for($i=0;$i<count($user_on_machine);$i++){
				$collector[$found]["user"][$i]=0;
			};
			$collector[$found]["date"]=$row["date"];
			//echo "date:".$collector[$found]["date"]." data:".$collector[$found]["data"]."<br>";
		};

		// fill data
		for($i=0;$i<count($user_on_machine);$i++){
			//echo "row[user_id]=".$row["user_id"]." == ".$user_on_machine[$i]["id"]."<br>";
			if($row["user_id"]==$user_on_machine[$i]["id"]){
				$collector[$found]["user"][$i]=floor($row["used_time"]/60);
				//echo "set<br>";
				break;
			};

			//echo "found ".$found.",date:".$collector[$found]["date"]." data:".$collector[$found]["user"][$i]."<br>";

		};

	};


	// sum up the data as string
	$data="";
	//echo "<hr><hr>".$db_mach[$row2["id"]]["name"]."<hr>";
	for($i=0;$i<count($collector);$i++){
		$data.='{ "category": "'.date("Y-m-d",$collector[$i]["date"]).'"';
		for($ii=0;$ii<count($user_on_machine);$ii++){
			$data.=', "u'.$user_on_machine[$ii]["id"].'":'.$collector[$i]["user"][$ii];
		};
		$data.='},';
	};

	// remove trailing ","
	if(strlen($data)>1){
		$data=substr($data,0,strlen($data)-1);
	};

	// generate config section (which user are on this graph)
	$g_config=gen_config($user_on_machine);

	$ret[0]=gen_chart($g_config,$data,$db_mach[$machine_id]["name"]);
	$ret[1]='<div id="'.$db_mach[$machine_id]["name"].'" style="width: 100%; height: 300px; background-color: #FFFFFF;" ></div>';
	$ret[2]=$log_entries;

	return $ret;

};


function gen_config($graph_config){
	$g_config="";
	for($i=0; $i<count($graph_config); $i++){
		$g_config.='{
			"balloonText": "Usage of [[title]] at the [[category]]: [[value]] min",
			"bullet": "round",
			"id": "'.$graph_config[$i]["id"].'",
			"title": "'.$graph_config[$i]["title"].'",
			"valueField": "u'.$graph_config[$i]["id"].'"
			}';
		if($i<count($graph_config)-1){
			$g_config.=',';
		}
	};
	return $g_config;
};


function gen_chart($g_config,$g_data,$name){
	$ret='<script type="text/javascript">
	AmCharts.makeChart("'.$name.'",
		{
			"type": "serial",
			"categoryField": "category",
			"plotAreaFillAlphas": 0.2,
			"aaplotAreaFillColors": "#AEA9A9",
			"backgroundAlpha": 0.56,
			"aabackgroundColor": "#b5b3b3",
			"startDuration": 1,
			"chartCursor": {},
			"chartScrollbar": {},
			"categoryAxis": {
				"gridPosition": "start"
			},
			"trendLines": [],
			"graphs": [ '.$g_config.' ],
			"guides": [],
			"valueAxes": [{
				"id": "ValueAxis-1",
				"title": "Usage [min]"
				}],
			"allLabels": [],
			"balloon": {},
			"legend": {
				"useGraphSettings": true,
				"backgroundAlpha": 0.56,
				"aabackgroundColor": "#b5b3b3",
				"borderColor": "#E10E0E"
			},
			"titles": [{
				"id": "Title-1",
				"size": 15,
				"text": "Usage of '.$name.'"
				}],
			"dataProvider": [ '.$g_data.' ]
		});
		</script>';
	return $ret;
};



echo '<script type="text/javascript" src="../js/amcharts.js"></script><script type="text/javascript" src="../js/serial.js"></script>';


$o_usage="";
$o_data="";
$o_show="";
$req2 = 'SELECT `id` FROM `mach` where active=1';
$stmt2 = $db->prepare($req2);
$stmt2->execute();
foreach($stmt2 as $row2){
	//echo "<br>taking a look at this machine ".$db_mach[$row2["id"]]["name"]."<br>";
	$data=get_graph($row2["id"]);
	if($data[2]>0){
		$o_data.=$data[0];
		$o_show.=$data[1];
	};

};

$o_usage=$o_data.$o_show;



?>
