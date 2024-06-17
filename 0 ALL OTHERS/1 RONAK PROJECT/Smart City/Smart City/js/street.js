setInterval(get_street_light,500);
function get_street_light() {
		firebase.database().ref('street_light/').once('value').then(function(snapshot){
		var l1=snapshot.val().l1;
		var l2=snapshot.val().l2;
		var l3=snapshot.val().l3;
		var l4=snapshot.val().l4;
		console.log(l1);
		console.log(l2);
		console.log(l3);
		console.log(l4);

    	if(l1=="1"){
    		document.getElementById('bulb_off4').src='Images/on.png';
    		document.getElementById('bulb_off3').src='Images/off.png';
    		document.getElementById('bulb_off2').src='Images/off.png';
    		document.getElementById('bulb_off1').src='Images/off.png';
			
		}else if(l2=="1"){
			document.getElementById('bulb_off4').src='Images/off.png';
    		document.getElementById('bulb_off3').src='Images/on.png';
    		document.getElementById('bulb_off2').src='Images/off.png';
    		document.getElementById('bulb_off1').src='Images/off.png';
			
		}else if(l3=="1"){
			document.getElementById('bulb_off4').src='Images/off.png';
    		document.getElementById('bulb_off3').src='Images/off.png';
    		document.getElementById('bulb_off2').src='Images/on.png';
    		document.getElementById('bulb_off1').src='Images/off.png';
			
		}else if(l4=="1"){
			document.getElementById('bulb_off4').src='Images/off.png';
    		document.getElementById('bulb_off3').src='Images/off.png';
    		document.getElementById('bulb_off2').src='Images/off.png';
    		document.getElementById('bulb_off1').src='Images/on.png';
			
		}else{
			document.getElementById('bulb_off4').src='Images/off.png';
    		document.getElementById('bulb_off3').src='Images/off.png';
    		document.getElementById('bulb_off2').src='Images/off.png';
    		document.getElementById('bulb_off1').src='Images/off.png';
		}
	})
}



