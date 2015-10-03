<?php

$header='<!DOCTYPE html><html><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="x-dns-prefetch-control" content="off">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta name="robots" content="noindex">
<title>MACS - Dashboard</title>
<link rel="stylesheet" href="css/design.css" type="text/css" media="screen" charset="utf-8">	
<link rel="stylesheet" href="css/jquery-ui.css"/>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.2/jquery.min.js"></script>
<script type=\'text/javascript\'>
	$(function(){
		$(\'.click\').click(function(){
			$(this).nextUntil(\'tr.spacer\').fadeToggle("slow");
			});
		$(\'.hideatstart\').nextUntil(\'tr.spacer\').hide();
		$(\'#info\').delay(2000).fadeOut(1500);
		$(\'input:text\').click( function(){
			if($(this).val()==\'-\'){
			        $(this).val(\'\');
			};
		});
		$(".conf").click(function(event) {
		        if( !confirm(\'Are you sure to delete this entry?\')){
		            event.preventDefault();
			};
		});


		});
</script></head>';

$footer='</body></html>';

?>

