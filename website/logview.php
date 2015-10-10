<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
date_default_timezone_set("UTC");

session_start();
require_once("helper.php");
//require_once("sec.php");
require_once("con.php");
require_once("data.php");
require_once("history.php");
require_once("html.php");

$DEBUG=0;

include("php_module/module_log.php");


$o=$header.'<table class="maintable">';
$o.='</td></tr><tr class="spacer"><td>&nbsp;</td></tr><tr class="header click"><td>+ Log '.$title.'<a name="log"></a></td></tr><tr><td>';
$o.='<tr><td>';

$o.=$o_log;

$o.='</td></tr></table>';



$o.=$footer;
echo $o;

?>
