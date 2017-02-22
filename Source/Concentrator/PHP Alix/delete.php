<?php
require_once ("include/config.php");
require_once ("include/checklogin.php");


//establish the connection
$con = mysql_connect($dbhost, $dbuser, $dbpassword);

if (!$con) {
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

        <li><a href="dashboard.php">Visualizar</a></li>
        <li><a href="change.php">Editar</a></li>
        <li><a href="insert.php">Inserir</a></li>
        <li><a class="active" href="delete.php">Eliminar</a></li>
        <li><a href="contacto.php">Contactos</a></li>
        <li><a href="logout.php">Sair</a></li>

</div>
<p </p
<div class="alterar">
<div class="boxinfo">
	
		<div class="boxleft">
			<div class="title">Selecionar Opção</div>
			<div class="options">
				<a href="?option=1"<?php if (isset($_GET["option"]) && $_GET["option"] == 1) echo ' class="active"'; ?>>Eliminar Nó de Sensorização</a>
				<a href="?option=2"<?php if (isset($_GET["option"]) && $_GET["option"] == 2) echo ' class="active"'; ?>>Eliminar Sensor</a>
				<a href="?option=3"<?php if (isset($_GET["option"]) && $_GET["option"] == 3) echo ' class="active"'; ?>>Eliminar Utilizador</a>
			</div>
			<div class="detail_options">
<?php


switch ($_GET["option"]) 
{
	case 1 :
		//Eliminar Nó de sensorização
?>
		<form name="node_sensor" method="post" action="">
			<strong>Selecionar Nó de Sensorização</strong>
			<p><select name="combo1">
			<option value="-">- Escolha um nó -</option>
<?php
		$query = mysql_query("SELECT * FROM store.Node");
		while ($row = mysql_fetch_array($query)) 
		{
   			echo '<option value= ' . $row["id_mac"] . '>' . $row["id_mac"] . ', ' . $row["descriptor"] .'</option>';  
   		}
?>

			</select></p>
			<input type="submit" name="submit" value="Eliminar" />
			<br /><br />
			</form>
					
					
<?php

		if (isset($_POST["submit"]) && $_POST["combo1"]!="-")
		{
			$query = mysql_query("DELETE FROM store.Node_has_Sensor WHERE Node_id_mac = '". $_POST["combo1"] ."'");
			$query = mysql_query("DELETE FROM store.Node WHERE id_mac = '". $_POST["combo1"] ."'");
			if ($query == 1)
					echo "<font color='green' size='2'>Nó de sensorização eliminado. </font>";
				else
					echo "<font color='red' size='2'>Erro. Tente novamente!</font>";
		}
		break;
		
	case 2 :
		//sensores
?>
		<form name="sensor" method="post" action="">
			<strong>Selecionar Tipo de Sensor</strong>
			<p><select name="combo1">
			<option value="-">- Escolha um sensor -</option>
<?php
		$query = mysql_query("SELECT * FROM store.Sensor");
		while ($row = mysql_fetch_array($query)) 
		{
   			echo '<option value= ' . $row["id_sensor"] . '>' . $row["id_sensor"] . ', ' . $row["descriptor"] .'</option>';  
   		}
?>

			</select></p>
			<input type="submit" name="submit" value="Eliminar" />
			<br /><br />
			</form>
					
					
<?php

		if (isset($_POST["submit"]) && $_POST["combo1"]!="-")
		{
			$query = mysql_query("DELETE FROM store.Node_has_Sensor WHERE Sensor_id_sensor = '". $_POST["combo1"] ."'");
			$query = mysql_query("DELETE FROM store.Sensor WHERE id_sensor = '". $_POST["combo1"] ."'");
			if ($query == 1)
					echo "<font color='green' size='2'>Nó de sensorização eliminado. </font>";
				else
					echo "<font color='red' size='2'>Erro. Tente novamente!</font>";
		}
		break;
		
	case 3:
		//utilizadores

	if ($_SESSION["username"]==root)
	{
?>
		<form name="sensor" method="post" action="">
			<strong>Selecionar Utilizador</strong>
			<p><select name="combo1">
			<option value="-">- Escolha um utilizador -</option>
<?php
		$query = mysql_query("SELECT * FROM store.User WHERE NOT username = 'root'");
		echo AQUI;
		echo $query; 
		while ($row = mysql_fetch_array($query)) 
		{
   			echo '<option value= ' . $row["username"] . '>' . $row["username"] . ', ' . $row["email"] .'</option>';  
   		}
?>

			</select></p>
			<input type="submit" name="submit" value="Eliminar" />
			<br /><br />
			</form>
					
					
<?php

		if (isset($_POST["submit"]) && $_POST["combo1"]!="-")
		{
			$query = mysql_query("DELETE FROM store.User WHERE username = '". $_POST["combo1"] ."'");
			echo $_POST["combo1"];
			if ($query == 1)
					echo "<font color='green' size='2'>Utilizador eliminado com sucesso. </font>";
				else
					echo "<font color='red' size='2'>Erro. Tente novamente!</font>";
		}
		break;	
	}
	else echo "<font color='red' size='2'>Somente o utilizador root pode realizar esta acção!</font>";
	break;

}
?>
			</div>
		</div>
	
	</div>      
</div>
