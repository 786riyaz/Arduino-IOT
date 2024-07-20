setInterval(get_water_level,500);
function get_water_level() {
		firebase.database().ref('water_level/').once('value').then(function(snapshot){
		var l1=snapshot.val().level;
		var l2=snapshot.val().moto_status;
		

    	if(l1=="1"){
    		document.getElementById('img').src='Images/water/tank2.png';
    		document.getElementById('percent').innerHTML="10%";	
		}else if(l1=="2"){
			document.getElementById('img').src='Images/water/tank3.png';
    		document.getElementById('percent').innerHTML="30%";
		}else if(l1=="3"){
			document.getElementById('img').src='Images/water/tank4.png';
    		document.getElementById('percent').innerHTML="50%";
		}else if(l1=="4"){
			document.getElementById('img').src='Images/water/tank5.png';
    		document.getElementById('percent').innerHTML="70%";
		}else if(l1=="5"){
			document.getElementById('img').src='Images/water/tank6.png';
    		document.getElementById('percent').innerHTML="90%";
    		
    	}else{
    		document.getElementById('img').src='Images/water/tank1.png';
    		document.getElementById('percent').innerHTML="0%";
    	}
    	if (l2=="1") {
    		document.getElementById('status').innerHTML="ON";	
    	}else{
    		document.getElementById('status').innerHTML="OFF";
    	}
	})
}



