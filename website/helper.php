<?php


function hide_table($name){
	global $_GET;
	$show="hideatstart";
	if(isset($_GET["show"])){
	        if($_GET["show"]==$name){ // TODO ARRAY
        	        $show="";
	        }
	};
	return $show;
};

function show_info($txt){
	global $info;
	global $info_txt;
	if(!empty($info_txt)){
		$info_txt.=",".$txt;
	} else {
		$info_txt=$txt;
	};

	$info='<div id="info" class="info">'.$info_txt.'</div>';
};



?>
