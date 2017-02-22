<?php

require("include/config.php");
$force_logged_users = true;
require("include/checklogin.php");

$error_login = (isset($_GET["error_login"])) ? $_GET["error_login"] : false;

if (isset($_POST["dologin"])) {
	
	//establish the connection
	$con = mysql_connect($dbhost, $dbuser, $dbpassword);
	
	if (!$con)
	{
		die('Could not connect: ' . mysql_error());
	}

	//select database
	mysql_select_db($dbname, $con);	
	
	//run query
	$result = mysql_query("SELECT * FROM store.User WHERE username = '" . $_POST["username"] . "' AND password = '" .$_POST["password"] . "' LIMIT 0, 1");

	$total_rows = mysql_num_rows($result);

	if ($total_rows==1) {
		
		session_start();
		while($row = mysql_fetch_array($result)){
			$_SESSION["username"] = $row["username"];
		}
		mysql_close($con);
		header("Location: $loggedpage");
		exit;
	}
	else if(($_POST["username"] =="") || ($_POST["password"] =="")){
		$error_login = 2;
	}
	else
		$error_login = 1;
	
	//close the connection
	mysql_close($con);
}


?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>InovRetail - In Store Retail Monitoring</title>
<link href="css/Weblogin.css" rel="stylesheet" type="text/css" />
</head>

<body onload="document.login.username.focus()"> <!--OOOOOOOOOOOOOOOOOOOOOOO-->

<div id="Box">
<h1>In Store Retail Monitoring</h1>
        <img src="images/logo.png" hspace="10" vspace="10" /> 
  
<?php

if ($error_login) {
?>
	<p class="error"><?php
    	switch($error_login) {
			case 1:
				echo "Os dados que introduziu não estão correctos. Por favor, tente novamente.";
			break;
			case 2:
				echo "Por favor, faça login!";
			break;
		}
	?></p>
<?php	
}

?>  

        
      <form  name="login" method="post" action="index.php">
	  <label for="username">Utilizador</label><br/>
	  <input type="text" name="username" /><br/>
      <label for="password">Senha</label><br/>
	  <input type="password" name="password" /><br/>
      <h3><input type="submit" name="dologin" value="Login" /></h3> 
</form>
        
<h3><a href="repairpassword.php">Esqueceu-se da password?</a></h3>

</div>
</body>
</html>
