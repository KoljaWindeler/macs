$(document).ready(function() {
	var old="";

	function get_data() {
		$.ajax({
			type: "GET",
			url: "php_module/module_log_content.php",
			success: function(data){
				if(data!=old){				
					var i=0;
					var lines=data.split(/\r\n|\r|\n/);
					var table=$("#logtable");
					// erase table
					$('.logentry').remove();

					//console.log("I found "+lines.length+" lines in the response");
					for(i=0; i<lines.length-1;i++){
						var col=lines[i].split(";");
						table.append('<tr class="logentry hl"><td>'+col[0]+'</td><td>'+col[1]+'</td><td>'+col[2]+'</td><td>'+col[3]+'</td><td>'+col[4]+'</td></tr>');
						//console.log("line "+i+" has "+col.length+" data, first "+col[0]);
					};
					old=data;
				};
			} // success
		});
	};

	setInterval(function(){get_data();}, 1000);
	

});
