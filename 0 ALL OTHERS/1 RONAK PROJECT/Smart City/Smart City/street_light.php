<?php
include 'header.php';
 ?>
 <!DOCTYPE html>
 <html>
 <head>
 	<title>Smart Street Light</title>
 	<style type="text/css">
 		body{
 			
 		}
 		.main{
 			margin-top: 15px;
 			 background: url(Images/road.png) no-repeat;
      		height: 700px;
      		width: auto;
      		margin-left: 25%;
 		}
 		.first{
 			
 			margin-left: 12%;
 		}
 		.second{
 			margin-left: 5%;
 			margin-top: 5%;
 			
 		}
 	</style>
 </head>
 <body>
 	<div class="main">
 	<div class="first">
 		<img src="Images/bulb_off.png" id="bulb_off1" height="7%" width="7%">
 		<img src="Images/bulb_off.png" id="bulb_off2" height="7%" width="7%" style="margin-left: 16%;">
 	</div>
 	
 	<div class="second">
 		<img src="Images/bulb_off.png" id="bulb_off3" height="7%" width="7%">
 		<img src="Images/bulb_off.png" id="bulb_off4" height="7%" width="7%" style="margin-left: 28%;">
 	</div>
 </div>

<!-- The core Firebase JS SDK is always required and must be listed first -->
    <!-- The core Firebase JS SDK is always required and must be listed first -->
<script src="https://www.gstatic.com/firebasejs/8.1.1/firebase-app.js"></script>
<script src="https://www.gstatic.com/firebasejs/8.1.1/firebase-auth.js"></script>
<script src="https://www.gstatic.com/firebasejs/8.1.1/firebase-database.js"></script>

<!-- TODO: Add SDKs for Firebase products that you want to use
     https://firebase.google.com/docs/web/setup#available-libraries -->
<script src="https://www.gstatic.com/firebasejs/8.1.1/firebase-analytics.js"></script>

<script>
  // Your web app's Firebase configuration
  // For Firebase JS SDK v7.20.0 and later, measurementId is optional
  var firebaseConfig = {
    apiKey: "AIzaSyAMToE4sciKkLpVbCToP5VcGQbOHOYJ2xs",
    authDomain: "smart-city-2883e.firebaseapp.com",
    databaseURL: "https://smart-city-2883e.firebaseio.com",
    projectId: "smart-city-2883e",
    storageBucket: "smart-city-2883e.appspot.com",
    messagingSenderId: "223430422852",
    appId: "1:223430422852:web:01c3a24264e6005af86711",
    measurementId: "G-1N487KZ5RP"
  };
  // Initialize Firebase
  firebase.initializeApp(firebaseConfig);
  firebase.analytics();
</script>
<script type="text/javascript" src="js/st_light.js"></script>
 </body>
 </html>