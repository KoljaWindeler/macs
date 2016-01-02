<?php
include_once('../sql_password.php');
$db_wifi="";
$status="";
$o_setup="";
$o_setup_below="";

//ini_set('display_errors',1);
//ini_set('display_startup_errors',1);
//error_reporting(-1);

$header='<!DOCTYPE html><html><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="x-dns-prefetch-control" content="off">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta name="robots" content="noindex">
<title>MACS - Dashboard</title>
<link rel="stylesheet" href="../css/jquery-ui.css"/>
<link rel="stylesheet" href="../css/iframe.css"/>';

$dbhost = 'localhost';
$dbuser = 'root';
$dbpass = $sql_pw;
$dbname = 'macs';

function backup_tables($host,$user,$pass,$name,$tables = '*')
{

    $link = mysql_connect($host,$user,$pass);
    mysql_select_db($name,$link);
    mysql_query("SET NAMES 'utf8'");

    //get all of the tables
    if($tables == '*')
    {
        $tables = array();
        $result = mysql_query('SHOW TABLES');
        while($row = mysql_fetch_row($result))
        {
            $tables[] = $row[0];
        }
    }
    else
    {
        $tables = is_array($tables) ? $tables : explode(',',$tables);
    }
    $return='';
    //cycle through
    foreach($tables as $table)
    {
        $result = mysql_query('SELECT * FROM '.$table);
        $num_fields = mysql_num_fields($result);

        $return.= 'DROP TABLE '.$table.';';
        $row2 = mysql_fetch_row(mysql_query('SHOW CREATE TABLE '.$table));
        $return.= "\n\n".$row2[1].";\n\n";

        for ($i = 0; $i < $num_fields; $i++) 
        {
            while($row = mysql_fetch_row($result))
            {
                $return.= 'INSERT INTO '.$table.' VALUES(';
                for($j=0; $j<$num_fields; $j++) 
                {
                    $row[$j] = addslashes($row[$j]);
                    $row[$j] = str_replace("\n","\\n",$row[$j]);
                    if (isset($row[$j])) { $return.= '"'.$row[$j].'"' ; } else { $return.= '""'; }
                    if ($j<($num_fields-1)) { $return.= ','; }
                }
                $return.= ");\n";
            }
        }
        $return.="\n\n\n";
    }

    //save file
    $fn = '../db_backup/db-backup-'.time().'-'.(md5(implode(',',$tables))).'.sql';
    $handle = fopen($fn,'w+');
    fwrite($handle,$return);
    fclose($handle);
    return '<a href="'.$fn.'">click to download database</a>';
}

if (isset($_GET['restore'])) {
	if (is_uploaded_file($_FILES['filename']['tmp_name'])) {
		if (move_uploaded_file($_FILES['filename']['tmp_name'], "../db_backup/".$_FILES['filename']['name'])) {
			$command = "mysql -uroot -p123 -hlocalhost -D macs < ".$_FILES['filename']['name']."; rm ".$_FILES['filename']['name'];
			$output = shell_exec($command);
			$result = "Import done";
		}
	}
} else if(isset($_GET["generate"])){
	$result = backup_tables($dbhost,$dbuser,$dbpass,$dbname);
} else {
	$result="";
}

	echo $header.'<table border="0" width="100%"><tr><td>
	<form enctype="multipart/form-data" method="POST" action="'.basename(__FILE__).'?generate">
	<input type="submit" name="submit" value="Generate Backup">
	</form>
	</td><td width="50%">
	<form enctype="multipart/form-data" method="POST" action="'.basename(__FILE__).'?restore">
	<input size="0" type="file" name="filename">
	<input type="submit" name="submit" value="Upload File">
	</form>
	</td></tr>
	<tr><td colspan="2"><center>'.$result.'</center></td></tr></table>';

?>
