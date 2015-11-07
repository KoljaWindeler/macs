<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
date_default_timezone_set("UTC");

session_start();
require_once("helper.php");
require_once("sec.php");
require_once("con.php");
require_once("data.php");
require_once("history.php");
require_once("html.php");

$DEBUG=0;
$resume=0;
////////////////// DB CHANGES ///////////////
	//var_dump($_POST);
	////////////////// USER ///////////////
	if(isset($_POST["edit"])){
		if($_POST["edit"]=="user"){ // edit or delte button
			if(isset($_POST["e_badge"])){
				$_POST["e_badge"]=intval($_POST["e_badge"]);
			};
			if(isset($_POST["submit"]) & $_POST["submit"]=="delete"){ // delete
				$stmt = $db->prepare("UPDATE  `macs`.`user` SET `active` = 0 WHERE  `user`.`id` =:id;");
				$stmt->bindParam(":id",$_POST["eid"],PDO::PARAM_INT);
				$stmt->execute();

				// mark all machines that were affected by this deletation as updateable
				$stmt = $db->prepare("SELECT `mach_id` FROM  `macs`.`access` WHERE  `user_id` =:id;");
				$stmt->bindParam(":id",$_POST["eid"],PDO::PARAM_INT);
				$stmt->execute();
				foreach($stmt as $row){
					set_mach_outdated($row["mach_id"],$db);
				};

				// delete all access
				$stmt = $db->prepare("DELETE FROM  `macs`.`access` WHERE  `user_id` =:id;");
				$stmt->bindParam(":id",$_POST["eid"],PDO::PARAM_INT);
				$stmt->execute();

				add_log("-",$_POST["eid"],"User deleted","-");
				show_info("User deleted");
			}
			elseif(isset($_POST["submit"]) & ($_POST["submit"]=="Add" || $_POST["submit"]=="Update")){
				$execute=0;
				// 1. check if the data make sense
				if($_POST["e_name"]=="-" or empty($_POST["e_name"])){
					$execute=0;
					$resume=1;
					show_info("You have to provide a name");
				} elseif($_POST["e_badge"]=="-" or empty($_POST["e_badge"]) or !is_numeric($_POST["e_badge"])){
					$execute=0;
					$resume=1;
					show_info("You have to provide a numeric Badge id");
				} elseif($_POST["e_email"]=="-" or empty($_POST["e_email"]) or !filter_var($_POST["e_email"], FILTER_VALIDATE_EMAIL)){
					$execute=0;
					$resume=1;
					show_info("You have to provide a valid email");
				} elseif($_POST["e_login"]!="" and $_POST["macs_pw_md5"]=="d41d8cd98f00b204e9800998ecf8427e"){ // d41d8cd98f00b204e9800998ecf8427e = md5("")
					$execute=0;
					$resume=1;
					show_info("No login without pw");
				} else {
					if($_POST["e_id"]!=0){ // update
						$stmt = $db->prepare("UPDATE  `macs`.`user` SET `name` = :name,`badge_id` = :badge_id,`email`=:email,`login`=:login,`hash`=:hash WHERE  `user`.`id` =:id;");
						$stmt->bindParam(":id",$_POST["e_id"],PDO::PARAM_INT);
						$execute=1;

						// mark all machines that were affected by this change as updateable
						$stmt2 = $db->prepare("SELECT `mach_id` FROM  `macs`.`access` WHERE  `user_id` =:id;");
						$stmt2->bindParam(":id",$_POST["e_id"],PDO::PARAM_INT);
						$stmt2->execute();
						foreach($stmt2 as $row){
							set_mach_outdated($row["mach_id"],$db);
						};

					} else {
						// run some checks before we accept the data
						// 2. check if there is already the same badge
						$stmt = $db->prepare("SELECT COUNT(*) FROM `macs`.`user` WHERE badge_id=:badge_id");
						$stmt->bindParam(":badge_id",$_POST["e_badge"],PDO::PARAM_INT);
						$stmt->execute();
						foreach($stmt as $row){
							if($row["COUNT(*)"]>0){
								$excute=0;
								$resume=1;
								add_log("-","-","Badge ID already in db, entry rejected","-");
								show_info("Entry rejected, duplicate badge");
							} else {
								$stmt = $db->prepare("INSERT INTO  `macs`.`user` (`name`,`badge_id`,`email`,`active`,`login`,`hash`) VALUE (:name,:badge_id,:email,1,:login,:hash)");
								$execute=1;
							};
						} // for each
					} // else 
				}
				
				if($execute==1){
					$stmt->bindParam(":name",$_POST["e_name"],PDO::PARAM_STR);
					$stmt->bindParam(":badge_id",$_POST["e_badge"],PDO::PARAM_STR);
					$stmt->bindParam(":email",$_POST["e_email"],PDO::PARAM_STR);
					$stmt->bindParam(":login",$_POST["e_login"],PDO::PARAM_STR);
					$stmt->bindParam(":hash",$_POST["macs_pw_md5"],PDO::PARAM_STR);
					$stmt->execute();
	
					load_data(); // reload as user has changed
					if($_POST["e_id"]!=0){
						add_log("-",$_POST["e_id"],"User updated","-");
						show_info("User updated");

						// mark all machines that were affected by this deletation as updateable
						$stmt = $db->prepare("SELECT `mach_id` FROM  `macs`.`access` WHERE  `user_id` =:id;");
						$stmt->bindParam(":id",$_POST["e_id"],PDO::PARAM_INT);
						$stmt->execute();
						foreach($stmt as $row){
							set_mach_outdated($row["mach_id"],$db);
						};

					} else {
						$stmt = $db->prepare("SELECT ID FROM `macs`.`user` ORDER BY ID desc LIMIT 0,1");
						$stmt->execute();
						foreach($stmt as $row){
							add_log("-",$row["ID"],"User created","-");
						};
						show_info("user created");
					}; // msg



				}; // excute ok?
			} // edit is handled locally below
		} // delete or edit


		elseif($_POST["edit"]=="mach"){
			if(isset($_POST["submit"]) & $_POST["submit"]=="delete"){ // delete
				// we are going to rename the machine id to the next freee nr above 900, get free nr
				$request = $db->prepare('SELECT MAX(mach_nr) FROM mach');
				$request->execute();
				foreach ($request as $row) {
					$new_mach_nr=$row["MAX(mach_nr)"]+1;
				};
				if($new_mach_nr<900){
					$new_mach_nr=900;
				};
				
				// now, relabel machine
				$stmt = $db->prepare("UPDATE  `macs`.`mach` SET `active` = 0, `mach_nr`=".$new_mach_nr." WHERE  `mach`.`id` =:id;");
				$stmt->bindParam(":id",$_POST["eid"],PDO::PARAM_INT);
				$stmt->execute();

				
				add_log($_POST["eid"],"-","Machine deleted","-");
				show_info("Machine deleted");
			}
			elseif(isset($_POST["submit"]) & ($_POST["submit"]=="Add" || $_POST["submit"]=="Update")){
				$execute=0;
				// check if the data make sense
				if($_POST["e_name"]=="-" or empty($_POST["e_name"])){
					$execute=0;
					$resume=1;
					show_info("You have to provide a name");
				} elseif($_POST["e_mach_nr"]=="-" or empty($_POST["e_mach_nr"])){
					$execute=0;
					$resume=1;
					show_info("You have to provide a Mach id");
				} elseif(!($_POST["e_mach_nr"]<128 && $_POST["e_mach_nr"]>0)){
					$execute=0;
					$resume=1;
					show_info("Valid range for Mach Nr 1-127");
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
						$stmt = $db->prepare("UPDATE  `macs`.`mach` SET `name` = :name,`mach_nr` = :mach_nr,`desc`=:desc WHERE  `mach`.`id` =:id;");
						$stmt->bindParam(":id",$_POST["e_id"],PDO::PARAM_INT);
						$execute=1;

						// insert entry that this machine might need an update
						set_mach_outdated($_POST["e_id"],$db);

					} else {
						$stmt = $db->prepare("SELECT COUNT(*) FROM `macs`.`mach` WHERE mach_nr=:machine_id");
						$stmt->bindParam(":machine_id",$_POST["e_mach_nr"],PDO::PARAM_INT);
						$stmt->execute();
						foreach($stmt as $row){
							if($row["COUNT(*)"]>0){
								$excute=0;
								$resume=1;
								add_log("-","-","Mach ID already in db, entry rejected","-");
								show_info("Entry rejected, duplicate id");
							} else {
								$stmt = $db->prepare("INSERT INTO  `macs`.`mach` (`name`,`mach_nr`,`desc`,`active`) VALUE (:name,:mach_nr,:desc,1)");
								$execute=1;
							};
						} // for each
					} // insert
				} // if check ok

				if($execute==1){
					$stmt->bindParam(":name",$_POST["e_name"],PDO::PARAM_STR);
					$stmt->bindParam(":mach_nr",$_POST["e_mach_nr"],PDO::PARAM_STR);
					$stmt->bindParam(":desc",$_POST["e_desc"],PDO::PARAM_STR);
					$stmt->execute();

					load_data(); // reload as user has changed
					if($_POST["e_id"]!=0){
					add_log($_POST["e_id"],"-","Machine updated","-");
						show_info("machine updated");
					} else {
						$stmt = $db->prepare("SELECT ID FROM `macs`.`mach` ORDER BY ID desc LIMIT 0,1");
						$stmt->execute();
						foreach($stmt as $row){
							add_log($row["ID"],"-","Machine created","-");
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

			// mark all machines as updateable
			$stmt = $db->prepare("SELECT `id` FROM  `macs`.`mach`");
			$stmt->execute();
			foreach($stmt as $row){
				set_mach_outdated($row["id"],$db);
			};


			add_log("-","-","access right updated","-");
			show_info("access right updated");
		};
	};
	////////////////// USER ///////////////
////////////////// DB CHANGES ///////////////


$o=$header.'<table class="maintable"><tr class="header click '.hide_table("user").'"><td>+ Add/Edit User</td></tr>
<tr><td><table class="fillme" id="usertable">
<tr class="subheader"><td>Name</td><td>eMail</td><td>Badge-ID</td><td>Admin access</td><td>last used</td><td>edit</td></tr>';


///////////////// GET USER ///////////////
$o_user="";
$stmt = $db->prepare('SELECT * FROM user where active=1');
$stmt->execute();
foreach ($stmt as $row) {
	$last_seen=date("Y/m/d H:i",$row["last_seen"]-$_SESSION['tz']);
	if($row["last_seen"]==0){
		$last_seen="-";
	};
	$admin_access='no';
	if(!empty($row["login"]) and !empty($row["hash"])){
		$admin_access='yes';
	};

	$o_user.='<tr class="hl">
		<td>'.$row["name"].'</td>
		<td>'.$row["email"].'</td>
		<td>'.$row["badge_id"].'</td>
		<td>'.$admin_access.'</td>
		<td>'.$last_seen.'</td>
		<td>
		<form method="POST" action="index.php?show=user">
		<input type="submit" name="submit" value="edit">
		<input type="submit" name="submit" value="delete" class="conf">
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
$e_login="";
$e_hash="";
$e_id=0;
$btn="Add";

if(isset($_POST["edit"])){
	if($_POST["edit"]=="user" AND !empty($_POST["eid"]) AND $_POST["submit"]=="edit"){
		$stmt = $db->prepare('SELECT * FROM user where id=:id');
		$stmt->execute(array('id' => $_POST["eid"]));
		foreach ($stmt as $row){
			$e_name=$row["name"];
			$e_badge=$row["badge_id"];
			$e_id=$row["id"];
			$e_email=$row["email"];
			$e_login=$row["login"];
		};
		$btn="Update";
	} elseif($_POST["edit"]=="user" AND ($_POST["submit"]=="Add" || $_POST["submit"]=="Update") AND $resume==1){
		$e_name=$_POST["e_name"];
		$e_email=$_POST["e_email"];
		$e_badge=$_POST["e_badge"];
		$e_login=$_POST["e_login"];
		$e_id=$_POST["e_id"];
		if($_POST["submit"]=="Update"){
			$btn="Update";
		};
	};
};


$o.='	<tr><td colspan="7">&nbsp;</td></tr>
	<tr class="subheader"><td>Name</td><td>eMail</td><td colspan="3">Provid login/pw for admin access</td><td>Badge-ID</td></tr>
	<form action="index.php?show=user" method="POST" id="login_form"><tr>
		<td><input type="text" name="e_name" value="'.$e_name.'"></td>
		<td><input type="text" name="e_email" value="'.$e_email.'"></td>
		<td colspan="3"><input type="text" name="e_login" value="'.$e_login.'"> <input type="password" name="macs_pw" value="" id="macs_pw"></td>
		<td><input type="text" name="e_badge" value="'.$e_badge.'"></td></tr>
	<input type="hidden" name="edit" value="user"><input type="hidden" name="e_id" value="'.$e_id.'"><input type="hidden" name="macs_pw_md5" id="macs_pw_md5" value="">
	<input type="password" name="password" id="password_fake" class="hidden" autocomplete="off" style="display: none;">
	<tr><td colspan="5"><input type="submit" name="submit" value="'.$btn.'"></td></tr></form></table>';
///////////////// EDIT USER ///////////////

$o.='</td></tr><tr class="spacer"><td>&nbsp;</td></tr><tr class="header click '.hide_table("mach").'"><td>+ Add/Edit Stations</td></tr><tr><td>';

/////////////// GET MACHINE ////////////////
$o.='<table class="fillme"><tr class="subheader">
<td>Name</td><td>Machine-Nr</td><td>Description</td><td>last used</td><td>&nbsp;</td></tr>';

$o_mach="";
$stmt = $db->prepare('SELECT * FROM mach WHERE `active`=1');
$stmt->execute();
foreach ($stmt as $row) {
	$last_seen=date("Y/m/d H:i",$row["last_seen"]-$_SESSION['tz']);
	if($row["last_seen"]==0){
		$last_seen="-";
	};

	$o_mach.='<tr class="hl">
		<td>'.$row["name"].'</td>
		<td>'.$row["mach_nr"].'</td>
		<td>'.$row["desc"].'</td>
		<td>'.$last_seen.'</td>
		<td>
		<form method="POST" action="index.php?show=mach">
		<input type="submit" name="submit" value="edit">
		<input type="submit" name="submit" value="delete" class="conf">
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
$e_mach_nr="-";
$e_desc="-";
$e_id=0;
$btn="Add";
if(isset($_POST["edit"])){
	if($_POST["edit"]=="mach" AND !empty($_POST["eid"]) AND $_POST["submit"]=="edit"){
		$stmt = $db->prepare('SELECT * FROM mach where id=:id');
		$stmt->execute(array('id' => $_POST["eid"]));
		foreach ($stmt as $row){
			$e_name=$row["name"];
			$e_mach_nr=$row["mach_nr"];
			$e_id=$row["id"];
			$e_desc=$row["desc"];
		};
		$btn="Update";
	} elseif($_POST["edit"]=="mach" AND ($_POST["submit"]=="Add" || $_POST["submit"]=="Update") AND $resume==1){
		$e_name=$_POST["e_name"];
		$e_mach_nr=$_POST["e_mach_nr"];
		$e_desc=$_POST["e_desc"];
		$e_id=$_POST["e_id"];
		if($_POST["submit"]=="Update"){
			$btn="Update";
		};

	};
};
$o.='<tr><td colspan="5">&nbsp;</td></tr><tr class="subheader">
  <td>Name</td><td>Machine-Nr</td><td>Description</td><td colspan="2">&nbsp;</td></tr><tr>
	<form method="POST" action="index.php?show=mach">
	<td><input type="text" name="e_name" value="'.$e_name.'"></td>
	<td><input type="text" name="e_mach_nr" value="'.$e_mach_nr.'"></td>
	<td><input type="text" name="e_desc" value="'.$e_desc.'"></td>
	<td colspan="2">-</td></tr>
	<input type="hidden" name="edit" value="mach"><input type="hidden" name="e_id" value="'.$e_id.'">
  <tr><td colspan="5"><input type="submit" name="submit" value="'.$btn.'"></form></td></tr>
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
	$o_conn.='<tr class="hl"><td>'.$row["name"].'</td>';
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

$o_conn.='<tr><td colspan="'.(1+$dimx).'">&nbsp;</td></tr>
	 <tr><td colspan="'.(1+$dimx).'"><input type="submit" name="vehicle" value="Add/Update"><input type="hidden" name="edit" value="eaccess"></form></td></tr></table>';

$o.=$o_conn;


//////////////// LOG /////////////////
include("php_module/module_log.php");

$o.='</td></tr><tr class="spacer"><td>&nbsp;</td></tr><tr class="header click"><td>+ Log '.$title.'<a name="log"></a></td></tr><tr><td>';
$o.='<tr><td>';

$o.=$o_log;
//////////////// LOG /////////////////

$o.='</td></tr></table>';


if(!empty($info)){
	$o.=$info;
};

$o.=$footer;
echo $o;

?>
