setInterval(get_waste_status,500);
function get_waste_status() {
	firebase.database().ref('smart_waste/').once('value').then(function(snapshot){
		var l1=snapshot.val().status;
		console.log(l1);
		var flag=0;
		
    	if(l1=="1" && flag==0){
    		
    		document.getElementById('txt').innerHTML="Dustbin is full and notification has sent!";
    		window.alert('Dustbin is Full!');
    		flag=1;
    	}else if (l1=="0" && flag==1) {}{
			document.getElementById('txt').innerHTML=" ";
			flag=0;
			
		}
	})
	}