$(document).ready(function() {
	var setup_old="";
	var setup_same_content=0;
	//alert("und los");
	function get_data_setup_helper() {
		$.ajax({
			type: "GET",
			url: "module_setup_helper_reader.php",
			success: function(data){
				if(data!=setup_old){		
					//alert(data);		
					var a=$("#setup_log");
					if(a.length){
						a.html("Output:<br>"+data);
					};
					setup_old=data;
					setup_same_content=0;
				} else {
					setup_same_content=setup_same_content+1;
				}
			} // success
		});
	};
	// call it for 50 sec
	if(setup_same_content<100){
		setInterval(function(){get_data_setup_helper();}, 500);
	};
	

});
