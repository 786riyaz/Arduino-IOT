function getInformation() {
	firebase.database().ref().once('value').then(function(snapshot){
		var s1=snapshot.val().S1;
		var s2=snapshot.val().S2;
		var s3=snapshot.val().S3;

    	if(s1=="1"){
    		
			document.getElementById('img1').src='Images/alloted_car.png';
			document.getElementById('img2').src='Images/slot2.jpg';
			document.getElementById('img3').src='Images/slot2.jpg';
		}else if(s2=="1"){
			document.getElementById('img1').src='Images/alloted_car.png';
			document.getElementById('img2').src='Images/alloted_car.png';
			document.getElementById('img3').src='Images/slot2.jpg';
		}else if(s3=="1"){
			document.getElementById('img1').src='Images/alloted_car.png';
			document.getElementById('img2').src='Images/alloted_car.png';
			document.getElementById('img3').src='Images/alloted_car.png';
		}else{
			document.getElementById('img1').src='Images/slot2.jpg';
			document.getElementById('img2').src='Images/slot2.jpg';
			document.getElementById('img3').src='Images/slot2.jpg';
		}
	})
}
/*function getData() {
	firebase.database().ref().once('value').then(function(snapshot){
		var s1=snapshot.val().S1;
		var s2=snapshot.val().S2;
		var s3=snapshot.val().S3;
		if(s1=="1"){
			document.getElementById("slot1").src="Allotted";
			document.getElementById("slot2").src="Empty";
			document.getElementById("slot3").src="Empty";
		}else if(s2=="1"){
			document.getElementById("slot1").innerHTML="Allotted";
			document.getElementById("slot2").innerHTML="Allotted";
			document.getElementById("slot3").innerHTML="Empty";
		}else if(s3=="1"){
			document.getElementById("slot1").innerHTML="Allotted";
			document.getElementById("slot2").innerHTML="Allotted";
			document.getElementById("slot3").innerHTML="Allotted";
		}else{
			document.getElementById("slot1").innerHTML="Empty";
			document.getElementById("slot2").innerHTML="Empty";
			document.getElementById("slot3").innerHTML="Empty";
		}
		
	})
}*/