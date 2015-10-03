<?php
require_once('con.php');
if(isset($_GET["mach_nr"])){
	if(isset($_GET["action"]) && $_GET["action"]=="update"){
		$stmt = $db->prepare("SELECT badge_id FROM `user` WHERE active=1 and id in (select user_id from access where mach_id=(select id from mach where mach_nr=:id))");
		$stmt->bindParam(":id",$_GET["mach_nr"],PDO::PARAM_INT);
		$stmt->execute();
		$csv="";
	        foreach($stmt as $row){
			$csv.=$row["badge_id"].",";
		};

		echo substr($csv,0,-1);

		$stmt = $db->prepare("INSERT INTO `macs`.`log` (`id`, `timestamp`, `user_id`, `machine_id`, `event`) VALUES ('', '".time()."', '0', (SELECT `id` FROM `mach` WHERE mach_nr=:id), 'Station updated')");
		$stmt->bindParam(":id",$_GET["mach_nr"],PDO::PARAM_INT);
		$stmt->execute();

		$stmt = $db->prepare("DELETE FROM `update_available` WHERE mach_id in (select id from mach where mach_nr=:id)");
		$stmt->bindParam(":id",$_GET["mach_nr"],PDO::PARAM_INT);
		$stmt->execute();
	} else {
		$stmt = $db->prepare("SELECT COUNT(*) FROM `update_available` WHERE mach_id in (select id from mach where mach_nr=:id)");
		$stmt->bindParam(":id",$_GET["mach_nr"],PDO::PARAM_INT);
		$stmt->execute();

		$txt="n";
		
	        foreach($stmt as $row){
			if($row["COUNT(*)"]>0){
				$txt="y";
				break;
			};
		};
		echo $txt;
	};
};

?>
