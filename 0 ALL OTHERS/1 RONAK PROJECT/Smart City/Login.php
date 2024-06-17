<html>
<head>
    <title>Login</title>
    <style type="text/css">
      body{
    margin: 0;
    padding: 0;
    background: url(Images/city1.jpg) no-repeat;
    background-size: cover;
    font-family: sans-serif;
    }
    .main{
  width:auto;
  height:auto;
  background: #4C4646;
  color: #fff;
  top:50%;
  left: 50%;
  position: absolute;
  transform: translate(-50%,-50%);
  box-sizing: border-box;
  padding: 35px 30px;
  border-radius: 20px;
}
.main p{
  margin: 0;
  padding: 0;
  font-weight: 20px;
}
.main input{
  width: 100%;
  margin-bottom: 20px;
}
.main input[type="Email"],input[type="password"]{
  border: none;
  border-bottom: 1px solid #fff;
  background: transparent;
  outline: none;
  height: 40px;
  color: #fff;
  font-size: 16px;
}
button{
    border: none;
    background: #000;
    outline: none;
    height: 40px;
    color: #fff;
    font-size: 18px;
    margin: auto;
    width: 50%;
    border-radius: 5px;
}
button:hover{
  cursor: pointer;
  background: #fff;
  color: #000;
}
.main a{
  text-decoration: none;
  font-size: 16px;
  line-height: 20px;
  color: white;

}
.main a:hover{
  cursor: pointer;
  
  color: #000;
}



    </style>
</head>
<body>

<form action="" enctype="multipart/form-data" method="post">
  <div class="main">
        <center><h1>Smart City</h1></center>   
        <p>Email:</p>
        <input type="email" id="userEmail" required="required"><br>

        <p>Password:</p>
        <input type="password" id="userPassword" required="required"><br>
        <center><br><button type="button" onclick="Signin();" >Login</button><br></center>
        <br><a href="createAccount.php">Don't Have an Account ?</a>     
    
    </div>
</form>



<!-- The core Firebase JS SDK is always required and must be listed first -->
    <!-- The core Firebase JS SDK is always required and must be listed first -->
<script src="https://www.gstatic.com/firebasejs/8.1.1/firebase-app.js"></script>
<script src="https://www.gstatic.com/firebasejs/8.1.1/firebase-auth.js"></script>
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

<script src="js/SignIn.js" type="text/javascript"></script>

</body>
</html>