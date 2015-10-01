<?php
$db = new PDO('mysql:dbname=macs;host=127.0.0.1;charset=utf8', 'root', 'EEkiM05$.');

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
?>
