<?php
include_once('sql_password.php');
$db = new PDO('mysql:dbname=macs;host=127.0.0.1;charset=utf8', 'root', $sql_pw);

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
?>
