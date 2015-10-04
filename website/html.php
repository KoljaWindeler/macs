<?php
$tz="";
if(isset($_GET['tz'])){
	$_SESSION['tz']=$_GET['tz']*60;
} elseif (!isset($_SESSION['tz'])){
	$tz='var now = new Date(); window.location.replace(window.location.href+"?tz="+now.getTimezoneOffset());';
};

$header='<!DOCTYPE html><html><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="x-dns-prefetch-control" content="off">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta name="robots" content="noindex">
<title>MACS - Dashboard</title>
<link rel="stylesheet" href="css/design.css" type="text/css" media="screen" charset="utf-8">	
<link rel="stylesheet" href="css/jquery-ui.css"/>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.2/jquery.min.js" type=\'text/javascript\'></script>
<script src="http://cdnjs.cloudflare.com/ajax/libs/jstimezonedetect/1.0.4/jstz.min.js" type=\'text/javascript\'>></script>
<script src="http://crypto-js.googlecode.com/svn/tags/3.0.2/build/rollups/md5.js" type=\'text/javascript\'></script>
<script type=\'text/javascript\'>
	$(function(){
		'.$tz.'

		$(\'.click\').click(function(){
			$(this).nextUntil(\'tr.spacer\').fadeToggle("slow");
			});
		$(\'.hideatstart\').nextUntil(\'tr.spacer\').hide();
		$(\'#info\').delay(5000).fadeOut(1500);
		$(\'input\').click( function(){
			if($(this).val()==\'-\'){
			        $(this).val(\'\');
			};
		});
		$(".conf").click(function(event) {
		        if( !confirm(\'Are you sure to delete this entry?\')){
		            event.preventDefault();
			};
		});

		$("#login_form").submit(function(event){
			var pw=$("#macs_pw").val();
			$("#macs_pw").val("");
			var passhash = CryptoJS.MD5(pw);
			$("#macs_pw_md5").val(passhash.toString(CryptoJS.enc.Hex));
			//event.preventDefault();
			
		});

		$(".hl").delegate(\'td\',\'mouseover mouseleave\', function(e) {
		    if (e.type == \'mouseover\') {
		      $(this).parent().addClass("hover");
		      $("colgroup").eq($(this).index()).addClass("hover");
		    }
		    else {
		      $(this).parent().removeClass("hover");
		      $("colgroup").eq($(this).index()).removeClass("hover");
		    }
		});
	});
</script></head><body>';

if(isset($_SESSION['ID']) && $_SESSION['ID']!=0){
	$header.='<div class="right"><a href="index.php?logout">Logout</a></div>';
}

$header.='<div class="logo_l1">welcome to </div><div class="logo_l2">M.A.C.S.</div><br>';

$footer='</body></html>';

?>

