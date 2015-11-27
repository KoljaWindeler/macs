<?php
include_once('../con.php');
$f="/tmp/macs_config.txt";
$myfile = fopen($f, "w") or die("Unable to open file!");


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

for($i=0; $i<3; $i++){
	if(isset($db_wifi[$i]["ssid"]) && isset($db_wifi[$i]["pw"]) && isset($db_wifi[$i]["type"])){
		if(!empty($db_wifi[$i]["ssid"]) && !empty($db_wifi[$i]["pw"])){
			$line=$i."	".$db_wifi[$i]["ssid"]."	".$db_wifi[$i]["pw"]."	".$db_wifi[$i]["type"]."\r\n";
			fwrite($myfile,$line);
		};
	};
};

fclose($myfile);
?>
