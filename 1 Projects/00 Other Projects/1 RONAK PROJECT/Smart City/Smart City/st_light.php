<?php
include 'header.php';
?>
 <!DOCTYPE html>
 <html>
 <head>
 	<title>Smart Street Light</title>
 <style type="text/css">
  body{
        width: auto;
        height: auto;
        margin: 0;
        padding: 0;
        font-family: sans-serif;
        background: #E0E1EF;
       }
   .main{
    
    background: #000;
   }
   .up{
    margin-top: 5%;
    margin-left: 10%;
   }
    .down{
    
    margin-left: 10%;
   }
   .image{
    height: 100px;
    width:auto;
    background: #282828;
   }
   .middle{
    background: #fff;
    width: auto;
    height: 2px;
   }
 </style>
 </head>
 <body> 
        <center><h2>Get The Information About Street Lights</h2></center>
    <div class="main">
      <div class="up">
        <img src="Images/off.png" id="bulb_off1" height="70px" width="70px" style="margin-left: 5%">
        <img src="Images/off.png" id="bulb_off2" height="70px" width="70px" style="margin-left: 30%;">
      </div>
      <div class="image">
        
      </div>
      <div class="middle"></div>
      <div class="image">
        
      </div>
      <div class="down">
        <img src="Images/off.png" id="bulb_off3" height="70px" width="70px"  style="margin-left: 25%;">
        <img src="Images/off.png" id="bulb_off4" height="70px" width="70px" style="margin-left: 30%;">
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
<script type="text/javascript" src="js/street.js"></script>

 </body>
 </html>