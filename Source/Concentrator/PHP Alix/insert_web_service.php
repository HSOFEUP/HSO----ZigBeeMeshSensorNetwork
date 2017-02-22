<?php

require_once ("include/config.php");
require_once ("include/checklogin.php");

	//establish the connection
	$con = mysql_connect($dbhost, $dbuser, $dbpassword);
	
	if (!$con)
	{
		die('Could not connect: ' . mysql_error());
	}

	//select database
	mysql_select_db($dbname, $con);
?>	

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>InovRetail - In Store Retail Monitoring</title>
<link href="css/site.css" rel="stylesheet" type="text/css" />

</head>

<body>
<h2>Olá <?php echo $_SESSION["username"] ?>!</h2>
<div class="header">
    	<table  width="90%" class="headertitle"> 
        	<tr>
          		<td> <img src="images/logo.png" name="imagem_logo" width="191" height="61" hspace="0" vspace="5" id="imagem_logo" longdesc="logo.png"/> </td>
				<td>In Store Retail Monitoring</td>
        	</tr> 
        </table>
</div>

<div id="options">

        <li><a href="dashboard.php">Pesquisar</a></li>
        <li><a href="change.php">Editar</a></li>
        <li><a href="insert.php">Inserir</a></li>
        <li><a href="contacto.php">Contactos</a></li>
        <li><a href="logout.php">Sair</a></li>

</div>


<?php
	$sql="UPDATE mydb.Unit SET web_service = '".$_POST[web_service]."'
	WHERE (id_unit = 'teste')";

	if (!mysql_query($sql,$con))
	  {
	  die('Error: ' . mysql_error());
	  }
	echo "Endereço de Web Service alterado com SUCESSO. Faça retroceder"."<br />";
	echo $_POST[web_service];

	mysql_close($con);
?>

