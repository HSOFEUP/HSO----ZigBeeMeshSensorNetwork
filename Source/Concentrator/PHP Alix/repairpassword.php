<?php

require("include/config.php");
$force_logged_users = true;
require("include/checklogin.php");

$error = false;

function genRandomString() {
    $length = 6;
    $characters = '0123456789abcdefghijklmnopqrstuvwxyz';
    $string = "";    

    for ($p = 0; $p < $length; $p++) {
        $string .= $characters[mt_rand(0, (strlen($characters)-1))];
    }
    return $string;
}

if (isset($_POST["action"]) && isset($_POST["email"]))
{
	
	if (preg_match("/^([a-zA-Z0-9])+([\.a-zA-Z0-9_-])*@([a-zA-Z0-9_-])+(\.[a-zA-Z0-9_-]+)*\.([a-zA-Z]{2,6})$/", $_POST["email"]))
	{
		//check database
		//establish the connection
		$con = mysql_connect($dbhost, $dbuser, $dbpassword);
		
		if (!$con)
			die('Could not connect: ' . mysql_error());
	
		//select database
		mysql_select_db($dbname, $con);	
		
		//run query
		$result = mysql_query("SELECT * FROM store.User WHERE email = '" . $_POST["email"] . "' LIMIT 0, 1");
	
		$total_rows = mysql_num_rows($result);
		
		if ($total_rows==1) {
			$username = "";
			while ($row = mysql_fetch_array($result)) {
				$username = $row["username"];
			}
			$newpassword = genRandomString();
			$query = mysql_query("UPDATE store.User SET password = '".$newpassword."' WHERE username = '".$username."'", $con);
			echo mysql_errno($con) . mysql_error($con) . "\n";
			//send email
			$headers = 'MIME-Version: 1.0' . "\r\n";
			$headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
			$headers .= 'From: andre.goncalves@inovretail.com' . "\r\n";
			mail($_POST["email"],utf8_decode("Recuperação de Password"),utf8_decode("O seu nome de utilizador é \"" . $username . "\" e a sua nova password é: $newpassword"),$headers);		
		}
		else
			$error = true;	
	}
	else
		$error = true;
}

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>InovRetail - In Store Retail Monitoring</title>
<link href="css/styles.css" rel="stylesheet" type="text/css" />
</head>

<body>

<div id="Box">
<h1>In Store Retail Monitoring</h1>
<h2>Recuperação de password</h2>
<img src="images/logo.png" hspace="10" vspace="10" />

<?php

if (isset($_POST["action"]) && isset($_POST["email"]) && !$error) {
	echo "<h4>A sua nova password foi enviada para " . $_POST["email"]  . "</h4>";		
}
else {
?>
<?php
}

if ($error)
	echo '<h5 class="error">O endereço de e-mail é inválido.</h5>';
?>

<h3>Indique o endereço de e-mail da sua conta</h3>
<form id="repairpassword" name="repairpassword" method="post" action="repairpassword.php">

	  <label for="email">E-mail</label><br />
	  <input type="text" name="email" /><br />
      <br />
      <input type="hidden" name="action" value="1" />
      <input type="submit" name="repairpassword" value="Enviar" />
</form>


<br />
<table align="center">
<tr>
<td>
<img src="images/voltar.png" />
</td>
<td>
<h3 id="botao">
<a href="index.php">Voltar à página inicial</a> 
</h3>
</td>
</tr>
</table>

</div>
</body>
</html>
