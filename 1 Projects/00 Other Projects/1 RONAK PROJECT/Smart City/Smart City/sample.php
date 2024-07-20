<!DOCTYPE html>
<html>
<head>
	<title>Smart City</title>
  <style type="text/css">
  body{
        margin: 0;
        padding: 0;
        font-family: sans-serif;
        background: url(Images/parking3.jpg) no-repeat;
        background-size: cover;

      }
  .slots{
     width:auto;
      height:auto;
      background: #282828;
      color: #fff;
      top:50%;
      left: 50%;
      position: absolute;
      transform: translate(-50%,-50%);
      box-sizing: border-box;
      padding: 35px 30px;
      border-radius: 10px;
    }
  button{
        border: none;
        background: #000;
        outline: none;
        height: 40px;
        color: #fff;
        font-size: 18px;
          margin: auto;
        width: 10%;
        border-radius: 5px;
    }
  button:hover{
        cursor: pointer;
        background: #ffff00;
        color: #000;
    }
  </style>
</head>
<body>
  <center><h2>Get The Infromation About Parking Slots</h2></center>
  <center><button type="button" onclick="getInformation();">Click Here</button></center>
<div class="slots">
  <center>
  
<table>
  <tr>
    <th><h3>Slot 1</h3></th>
    <th><h3>Slot 2</h3></th>
    <th><h3>Slot 3</h3></th>
  </tr>
  <tr>
    <td><img src="Images/slot2.jpg" id="img1" height="auto" width="auto"></td>
    <td><img src="Images/slot2.jpg" id="img2" height="auto" width="auto"></td>
    <td><img src="Images/slot2.jpg" id="img3" height="auto" width="auto"></td>
  </tr>
</table>
</center>
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
<script type="text/javascript" src="js/sample2.js"></script>
</body>
</html>