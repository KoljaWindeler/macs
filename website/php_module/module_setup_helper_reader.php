<?php
$file="/tmp/macs_log.txt";
$myfile = fopen($file, "r") or die("Unable to open file!");
$data=fread($myfile,filesize($file));
echo str_replace("\r\n","<br>",$data);
fclose($myfile);
?>
