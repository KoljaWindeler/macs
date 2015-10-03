<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);

require_once("con.php");
require_once("html.php");
require_once("data.php");
require_once("history.php");
require_once("helper.php");

$DEBUG=0;
$resume=0;
////////////////// DB CHANGES ///////////////
	#var_dump($_POST);
	////////////////// USER ///////////////
	if(isset($_POST["edit"])){
		if($_POST["edit"]=="user"){ // edit or delte button
			if(isset($_POST["submit"]) & $_POST["submit"]=="delete"){ // delete
				$stmt = $db->prepare("UPDATE  `macs`.`user` SET `active` = 0 WHERE  `user`.`id` =:id;");
				$stmt->bindParam(":id",$_POST["eid"],PDO::PARAM_INT);
				$stmt->execute();
				add_log("-",$_POST["eid"],"User deleted");
				show_info("User deleted");
			}
			elseif(isset($_POST["submit"]) & $_POST["submit"]=="Add/Update"){
				$execute=0;
				if($_POST["e_id"]!=0){ // update
					$stmt = $db->prepare("UPDATE  `macs`.`user` SET `name` = :name,`badge_id` = :badge_id,`email`=:email WHERE  `user`.`id` =:id;");
					$stmt->bindParam(":id",$_POST["e_id"],PDO::PARAM_INT);
					$execute=1;
				} else {
					// run some checks before we accept the data
					// 1. check if there is already the same badge
					$stmt = $db->prepare("SELECT COUNT(*) FROM `macs`.`user` WHERE badge_id=:badge_id");
					$stmt->bindParam(":badge_id",$_POST["e_badge"],PDO::PARAM_INT);
					$stmt->execute();
					foreach($stmt as $row){
						if($row["COUNT(*)"]>0){
							$excute=0;
							$resume=1;
							add_log("-","-","Badge ID already in db, entry rejected");
							show_info("Entry rejected, duplicate badge");
						} else {
							$stmt = $db->prepare("INSERT INTO  `macs`.`user` (`name`,`badge_id`,`email`,`active`) VALUE (:name,:badge_id,:email,1)");
							$execute=1;
						};
					} // for each
					// 2. check if the data make sense
					if($_POST["e_name"]=="-" or empty($_POST["e_name"])){
						$execute=0;
						$resume=1;
						show_info("You have to provide a name");
					} elseif($_POST["e_badge"]=="-" or empty($_POST["e_badge"])){
						$execute=0;
						$resume=1;
						show_info("You have to provide a Badge id");
					} elseif($_POST["e_email"]=="-" or empty($_POST["e_email"])){
						$execute=0;
						$resume=1;
						show_info("You have to provide a email");
					};

				}
				
				if($execute==1){
					$stmt->bindParam(":name",$_POST["e_name"],PDO::PARAM_STR);
					$stmt->bindParam(":badge_id",$_POST["e_badge"],PDO::PARAM_STR);
					$stmt->bindParam(":email",$_POST["e_email"],PDO::PARAM_STR);
					$stmt->execute();
	
					load_data(); // reload as user has changed
					if($_POST["e_id"]!=0){
						add_log("-",$_POST["e_id"],"User updated");
						show_info("User updated");
					} else {
						$stmt = $db->prepare("SELECT ID FROM `macs`.`user` ORDER BY ID desc LIMIT 0,1");
						$stmt->execute();
						foreach($stmt as $row){
							add_log("-",$row["ID"],"User created");
						};
						show_info("user created");
					}; // msg
				}; // excute ok?
			} // edit is handled locally below
		} // delete or edit


		elseif($_POST["edit"]=="mach"){
			if(isset($_POST["submit"]) & $_POST["submit"]=="delete"){ // delete
				// we are going to rename the machine id to the next freee nr above 900, get free nr
				$request = $db->prepare('SELECT MAX(mach_id) FROM mach');
				$request->execute();
				foreach ($request as $row) {
					$new_mach_id=$row["MAX(mach_id)"]+1;
				};
				if($new_mach_id<900){
					$new_mach_id=900;
				};
				
				// now, relabel machine
				$stmt = $db->prepare("UPDATE  `macs`.`mach` SET `active` = 0, `mach_id`=".$new_mach_id." WHERE  `mach`.`id` =:id;");
				$stmt->bindParam(":id",$_POST["eid"],PDO::PARAM_INT);
				$stmt->execute();

				
				add_log($_POST["eid"],"-","Machine deleted");
				show_info("Machine deleted");
			}
			elseif(isset($_POST["submit"]) & $_POST["submit"]=="Add/Update"){
				$execute=0;
				// check if the data make sense
				if($_POST["e_name"]=="-" or empty($_POST["e_name"])){
					$execute=0;
					$resume=1;
					show_info("You have to provide a name");
				} elseif($_POST["e_mach_id"]=="-" or empty($_POST["e_mach_id"])){
					$execute=0;
					$resume=1;
					show_info("You have to provide a Mach id");
				} elseif($_POST["e_desc"]=="-" or empty($_POST["e_desc"])){
					$execute=0;
					$resume=1;
					show_info("You have to provide a description");
				} elseif(strlen($_POST["e_name"])>12){
					$execute=0;
					$resume=1;
					show_info("Please limit name to 12 chars");
				} else {
					if($_POST["e_id"]!=0){ // update
						$stmt = $db->prepare("UPDATE  `macs`.`mach` SET `name` = :name,`mach_id` = :mach_id,`desc`=:desc WHERE  `mach`.`id` =:id;");
						$stmt->bindParam(":id",$_POST["e_id"],PDO::PARAM_INT);
						$execute=1;
					} else {
						$stmt = $db->prepare("SELECT COUNT(*) FROM `macs`.`mach` WHERE mach_id=:machine_id");
						$stmt->bindParam(":machine_id",$_POST["e_mach_id"],PDO::PARAM_INT);
						$stmt->execute();
						foreach($stmt as $row){
							if($row["COUNT(*)"]>0){
								$excute=0;
								$resume=1;
								add_log("-","-","Mach ID already in db, entry rejected");
								show_info("Entry rejected, duplicate id");
							} else {
								$stmt = $db->prepare("INSERT INTO  `macs`.`mach` (`name`,`mach_id`,`desc`,`active`) VALUE (:name,:mach_id,:desc,1)");
								$execute=1;
							};
						} // for each
					} // insert
				} // if check ok

				if($execute==1){
					$stmt->bindParam(":name",$_POST["e_name"],PDO::PARAM_STR);
					$stmt->bindParam(":mach_id",$_POST["e_mach_id"],PDO::PARAM_STR);
					$stmt->bindParam(":desc",$_POST["e_desc"],PDO::PARAM_STR);
					$stmt->execute();

					load_data(); // reload as user has changed
					if($_POST["e_id"]!=0){
					add_log($_POST["e_id"],"-","Machine updated");
						show_info("machine updated");
					} else {
						$stmt = $db->prepare("SELECT ID FROM `macs`.`mach` ORDER BY ID desc LIMIT 0,1");
						$stmt->execute();
						foreach($stmt as $row){
							add_log($row["ID"],"-","Machine created");
						};
						show_info("machine created");
					};
				}
			} // add update
		}
		elseif($_POST["edit"]=="eaccess"){
			$stmt = $db->prepare("TRUNCATE TABLE  `access`");
			$stmt->execute();
			foreach($_POST as $post => $value){
				$post_ex=explode("_",$post);
				if($post_ex[0]=="c" and count($post_ex)==3){
					$stmt=$db->prepare("INSERT INTO `macs`.`access` (`user_id`, `mach_id`) VALUES (:uid, :mid);");
					$stmt->bindParam(":uid",$post_ex[2],PDO::PARAM_INT);
					$stmt->bindParam(":mid",$post_ex[1],PDO::PARAM_INT);
					$stmt->execute();
				}
			};
			add_log("-","-","access right updated");
			show_info("access right updated");
		};
	};
	////////////////// USER ///////////////
////////////////// DB CHANGES ///////////////


$o=$header.'<body><div class="logo_l1">welcome to </div><div class="logo_l2">M.A.C.S.</div><br>
<div id="clients"></div>
<table class="maintable"><tr class="header click '.hide_table("user").'"><td>+ Add/Edit User</td></tr>
<tr><td><table class="fillme" id="usertable">
<tr class="subheader"><td>Name</td><td>eMail</td><td>Badge-ID</td><td>last used</td><td>edit</td></tr>';


///////////////// GET USER ///////////////
$o_user="";
$stmt = $db->prepare('SELECT * FROM user where active=1');
$stmt->execute();
foreach ($stmt as $row) {
  $o_user.='<tr>
		<td>'.$row["name"].'</td>
		<td>'.$row["email"].'</td>
		<td>'.$row["badge_id"].'</td>
		<td>'.date("Y/m/d H:i",$row["last_seen"]).'</td>
		<td>
		<form method="POST" action="index.php?show=user">
		<input type="submit" name="submit" value="edit">
		<input type="submit" name="submit" value="delete">
		<input type="hidden" name="eid" value="'.$row["id"].'" >
		<input type="hidden" name="edit" value="user" >
		</form>
	</td></tr>';
};
$o.=$o_user;
///////////////// GET USER ///////////////

///////////////// EDIT USER ///////////////
$e_name="-";
$e_badge="-";
$e_email="-";
$e_id=0;
if(isset($_POST["edit"])){
	if($_POST["edit"]=="user" AND !empty($_POST["eid"]) AND $_POST["submit"]=="edit"){
		$stmt = $db->prepare('SELECT * FROM user where id=:id');
		$stmt->execute(array('id' => $_POST["eid"]));
		foreach ($stmt as $row){
			$e_name=$row["name"];
			$e_badge=$row["badge_id"];
			$e_id=$row["id"];
			$e_email=$row["email"];
		};
	} elseif($_POST["edit"]=="user" AND $_POST["submit"]=="Add/Update" AND $resume==1){
		$e_name=$_POST["e_name"];
		$e_email=$_POST["e_email"];
		$e_badge=$_POST["e_badge"];
	};
};

$o.='	<tr><td colspan="5">&nbsp;</td></tr>
	<tr class="subheader"><td>Name</td><td>eMail</td><td>Badge-ID</td><td colspan="2">&nbsp;</td></tr>
	<tr><form action="index.php?show=user" method="POST">
		<td><input type="text" name="e_name" value="'.$e_name.'"></td>
		<td><input type="text" name="e_email" value="'.$e_email.'"></td>
		<td><input type="text" name="e_badge" value="'.$e_badge.'"></td>
		<td colspan="2">-</td></tr>
	<input type="hidden" name="edit" value="user"><input type="hidden" name="e_id" value="'.$e_id.'">
	<tr><td colspan="5"><input type="submit" name="submit" value="Add/Update"></form></td></tr>
	</table>';
///////////////// EDIT USER ///////////////

$o.='</td></tr><tr class="spacer"><td>&nbsp;</td></tr><tr class="header click '.hide_table("mach").'"><td>+ Add/Edit Stations</td></tr><tr><td>';

/////////////// GET MACHINE ////////////////
$o.='<table class="fillme"><tr class="subheader">
<td>Name</td><td>Machine-ID</td><td>Description</td><td>last used</td><td>&nbsp;</td></tr>';

$o_mach="";
$stmt = $db->prepare('SELECT * FROM mach WHERE `active`=1');
$stmt->execute();
foreach ($stmt as $row) {
  $o_mach.='<tr>
		<td>'.$row["name"].'</td>
		<td>'.$row["mach_id"].'</td>
		<td>'.$row["desc"].'</td>
		<td>'.date("Y/m/d H:i",$row["last_seen"]).'</td>
		<td>
		<form method="POST" action="index.php?show=mach">
		<input type="submit" name="submit" value="edit">
		<input type="submit" name="submit" value="delete">
		<input type="hidden" name="eid" value="'.$row["id"].'" >
		<input type="hidden" name="edit" value="mach" >
		</form>
		</td>

	</tr>';
};
$o.=$o_mach;

/////////////// GET MACHINE ////////////////
/////////////// EDIT MACHINE ////////////////
$e_name="-";
$e_mach_id="-";
$e_desc="-";
$e_id=0;
if(isset($_POST["edit"])){
	if($_POST["edit"]=="mach" AND !empty($_POST["eid"]) AND $_POST["submit"]=="edit"){
		$stmt = $db->prepare('SELECT * FROM mach where id=:id');
		$stmt->execute(array('id' => $_POST["eid"]));
		foreach ($stmt as $row){
			$e_name=$row["name"];
			$e_mach_id=$row["mach_id"];
			$e_id=$row["id"];
			$e_desc=$row["desc"];
		};
	} elseif($_POST["edit"]=="mach" AND $_POST["submit"]=="Add/Update" AND $resume==1){
		$e_name=$_POST["e_name"];
		$e_mach_id=$_POST["e_mach_id"];
		$e_desc=$_POST["e_desc"];
		$e_id=$_POST["e_id"];
	};
};
$o.='<tr><td colspan="5">&nbsp;</td></tr><tr class="subheader">
  <td>Name</td><td>Machine-ID</td><td>Description</td><td colspan="2">&nbsp;</td></tr><tr>
	<form method="POST" action="index.php?show=mach">
	<td><input type="text" name="e_name" value="'.$e_name.'"></td>
	<td><input type="text" name="e_mach_id" value="'.$e_mach_id.'"></td>
	<td><input type="text" name="e_desc" value="'.$e_desc.'"></td>
	<td colspan="2">-</td></tr>
	<input type="hidden" name="edit" value="mach"><input type="hidden" name="e_id" value="'.$e_id.'">
  <tr><td colspan="5"><input type="submit" name="submit" value="Add/Update" checked></form></td></tr>
  </table>';
/////////////// EDIT MACHINE ////////////////

/////////////// GET CONNECTION //////////////
$o.='</td></tr><tr class="spacer"><td>&nbsp;</td></tr><tr class="header click"><td>+ Connect User to Station</td></tr> <tr><td>
  <table class="fillme"><tr class="subheader" ><th class="">&nbsp;</td>';

$o_conn="";

// generate dimensions
$request = $db->prepare('SELECT MAX(ID) FROM mach order by id asc');
$request->execute();
foreach ($request as $row) {
	$matrix_dimx=$row["MAX(ID)"];
};
$request = $db->prepare('SELECT MAX(ID) FROM user order by id asc');
$request->execute();
foreach ($request as $row) {
	$matrix_dimy=$row["MAX(ID)"];
};

$request = $db->prepare('SELECT COUNT(*) FROM mach WHERE `active`=1 order by id asc ');
$request->execute();
foreach ($request as $row) {
	$dimx=$row["COUNT(*)"];
};
$request = $db->prepare('SELECT COUNT(*) FROM user WHERE `active`=1 order by id asc');
$request->execute();
foreach ($request as $row) {
	$dimy=$row["COUNT(*)"];
};
	
// machines
$i=0;
$mach = $db->prepare('SELECT * FROM mach WHERE `active`=1 order by id asc');
$mach->execute();
foreach ($mach as $mrow) {
	$o_conn.='<th class="rotate"><div><span>'.$mrow['name'].'</span></div></th>';
	$marray[$i]=$mrow;
	if($DEBUG) "marray[".$i."][id]=".$marray[$i]["id"]."<br>";
	$i++;
}
$o_conn.='</tr>';

// user
$user = $db->prepare('SELECT * FROM user WHERE `active`=1 order by id asc');
$user->execute();

// generate access matrix
if($DEBUG) "generate access matrix (".$matrix_dimx."/".$matrix_dimy.")<br>";
for($x=0;$x<=$matrix_dimx;$x++){
	for($y=0;$y<=$matrix_dimy;$y++){
		$access[$x][$y]=0;  // x=machine , y=user
	}
};

$db_a = $db->prepare('SELECT * FROM access order by id asc');
$db_a->execute();
foreach ($db_a as $arow) {
	$access[$arow['mach_id']][$arow['user_id']]=1;
}

if($DEBUG) "matrix:<br>";
for($x=0;$x<=$matrix_dimx;$x++){
	for($y=0;$y<=$matrix_dimy;$y++){
		if($DEBUG) $access[$x][$y].',';
	}
	if($DEBUG) '<br>';
};

// output
$i=0;
$o_conn.='<form action="index.php" method="POST">';
foreach ($user as $row) {
	$o_conn.='<tr><td>'.$row["name"].'</td>';
	for($x=0;$x<$dimx;$x++){
		$checked="";
		if($DEBUG) "searching in access[".$marray[$x]["id"]."][".$row["id"]."]<br>";
		if($access[$marray[$x]["id"]][$row["id"]]==1){
			$checked="checked";
		};
		$o_conn.='<td><input type="checkbox" name="c_'.$marray[$x]["id"].'_'.$row["id"].'" value="1" '.$checked.'></td>';
	}
	$i++;
};

$o_conn.='<tr><td colspan="'.(1+count($mach)).'">&nbsp;</td></tr>
	 <tr><td colspan="'.(1+count($mach)).'"><input type="submit" name="vehicle" value="Add/Update"><input type="hidden" name="edit" value="eaccess"></form></td></tr></table>';

$o.=$o_conn;


//////////////// LOG /////////////////
$title="(Newestest 20 entries - <a href='index.php?logall'>show all</a>)";
$sql_limit="LIMIT 0,20";
if(isset($_GET["logall"])){
	$title="(all)";
	$sql_limit="";
}
$o.='</td></tr><tr class="spacer"><td>&nbsp;</td></tr><tr class="header click"><td>+ Log '.$title.'</td></tr><tr><td>';
$o.='<tr><td><table class="fillme" id="logtable"><tr class="subheader"><td>Time</td><td>Machine</td><td>User</td><td>Event</td></tr>';

$o_log="";
$stmt = $db->prepare('SELECT * FROM log ORDER BY timestamp desc '.$sql_limit);
$stmt->execute();
foreach ($stmt as $row) {
	$u_out="-";
	$m_out="-";

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
	}
	

 	 $o_log.='<tr>
		<td>'.date("Y/m/d H:i",$row["timestamp"]).'</td>
		<td>'.$m_out.'</td>
		<td>'.$u_out.'</td>
		<td>'.$row["event"].'</td>
		</tr>';

};
$o.=$o_log;

//////////////// LOG /////////////////

$o.='</td></tr></table></td></tr></table>';


if(!empty($info)){
	$o.=$info;
};

$o.=$footer;
echo $o;

?>
