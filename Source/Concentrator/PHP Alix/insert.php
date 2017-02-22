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
        <li><a class="active" href="insert.php">Inserir</a></li>
        <li><a href="delete.php">Eliminar</a></li>
        <li><a href="contacto.php">Contactos</a></li>
        <li><a href="logout.php">Sair</a></li>

</div>
<p </p
<div class="alterar">
<div class="boxinfo">
	
		<div class="boxleft">
			<div class="title">Selecionar Opção</div>
			<div class="options">
				<a href="?option=1"<?php if (isset($_GET["option"]) && $_GET["option"] == 1) echo ' class="active"'; ?>>Inserir Nó</a>
				<a href="?option=2"<?php if (isset($_GET["option"]) && $_GET["option"] == 2) echo ' class="active"'; ?>>Inserir Sensor</a>
				<a href="?option=3"<?php if (isset($_GET["option"]) && $_GET["option"] == 3) echo ' class="active"'; ?>>Inserir Utilizador</a>
				<a href="?option=4"<?php if (isset($_GET["option"]) && $_GET["option"] == 4) echo ' class="active"'; ?>>Nó de Sensorização (Nó+Sensor)</a>
			</div>
			<div class="detail_options">
<?php


switch ($_GET["option"]) 
{
	case 1 :
		//Inserir Nó
?>		
		<form method="post" action="">
<?php


		echo '</select>
						<br />
						ID do nó: <input type="text" name="id_mac" />
						<br /><br />';
		echo '</select>
						Tempo de Aquisição: <input type="text" name="time"/>
						<br /><br />';

		echo '</select>
						Descrição do nó (opcional): <input type="text" name="descriptor" size="70" />
						<br /><br />';
?>
						<input type="submit" name="submit" value="Submeter" />
						<br /><br />
					</form>
					
<?php


		if (isset ($_POST["submit"])) 
		{
			//foi pedido para inserir na BD
			$query = mysql_query("INSERT INTO store.Node VALUES ('" . $_POST["id_mac"] . "',  '" . $_POST["time"] . "', '-1', '" . $_POST["descriptor"] . "')");
			if ($query == 1)
				echo "<font color='green' size='2'>Criado novo Nó. </font>";
			else
				echo "<font color='red' size='2'>Dados inseridos incorretamente. Verifique todos os campos. </font>";
		}
		break;
		
	case 2 :
		//sensores
?>		
		<form method="post" action="">
<?php


		echo '</select>
						<br />
						ID do sensor: <input type="text" name="id_sensor" />
						<br /><br />';

		echo '</select>
						Descrição do nó (opcional): <input type="text" name="descriptor" size="70" />
						<br /><br />';
?>
						<input type="submit" name="submit" value="Submeter" />
						<br /><br />
					</form>
					
<?php

		if (isset ($_POST["submit"])) 
		{
			//foi pedido para inserir na BD
			$query = mysql_query("INSERT INTO store.Sensor VALUES ('" . $_POST["id_sensor"] . "',  '" . $_POST["descriptor"] . "')");
			if ($query == 1)
				echo "<font color='green' size='2'>Criado novo Sensor. </font>";
			else
				echo "<font color='red' size='2'>Dados inseridos incorretamente. Verifique todos os campos. </font>";
		}
		break;
		
	case 3:
		//utilizadores
?>		
		<form method="post" action="">
<?php

		echo '</select>
				<br />
				Username: <input type="text" name="username" />
				<br /><br />';
		echo '</select>
				Passoword: <input type="password" name="new_password1" />';
		echo '</select>
				Repita a Passoword: <input type="password" name="new_password2" />
				<br /><br />';
		echo '</select>
				Email: <input type="text" name="email" size="70" />
				<br /><br />';
?>
						<input type="submit" name="submit" value="Submeter" />
						<br /><br />
					</form>
					
<?php


		if (isset ($_POST["submit"]) && ($_POST["new_password1"] != $_POST["new_password2"])) 
		{
			echo "<font color='red' size='2'>Passwords não coincidem!</font>";
		}
		if (isset ($_POST["submit"]) && ($_POST["new_password1"] == $_POST["new_password2"])) 
		{
				$query = mysql_query("INSERT INTO store.User VALUES ('" . $_POST["username"] . "', password = '" . $_POST["new_password1"] . "', email =  '" . $_POST["email"] . "')");
				if ($query == 1)
					echo "<font color='green' size='2'>Criado novo Utilizador. </font>";
				else
					echo "<font color='red' size='2'>Erro ao criar novo Utilizador. Verifique todos os campos. </font>";
				//session_start();
				//session_destroy();
				//header("Location: $initialpage");
				//exit;
		}
		break;

	case 4 :
		//Atribuição Nó+Sensor


?>
		<form name="node_sensor" method="post" action="">
			<strong>Nó + Sensor = Nó de Sensorização</strong>
			<p><select name="combo1">
			<option value="-">- Escolha um nó -</option>
<?php
		$query = mysql_query("SELECT * FROM store.Node");
		while ($row = mysql_fetch_array($query)) 
		{
   			echo '<option value= ' . $row["id_mac"] . '>' . $row["id_mac"] . ', ' . $row["descriptor"] .'</option>';  
   		}
?>
			</select>
		
		<p><select name="combo2">
		<option value="-">- Escolha um sensor -</option>
<?php
		$query = mysql_query("SELECT * FROM store.Sensor");
		while ($row = mysql_fetch_array($query)) 
		{
   			echo '<option value= ' . $row["id_sensor"] . '>' . $row["id_sensor"] . ', ' . $row["descriptor"] .'</option>';  
   		 }
?>
			</select></p>
			<input type="submit" name="submit" value="Actualizar" />
			<br /><br />
			</form>
					
					
<?php

		if (isset($_POST["submit"]) && $_POST["combo1"]!="-" && $_POST["combo2"]!="-")
		{
			$query = mysql_query("INSERT INTO store.Node_has_Sensor (Node_id_mac, Sensor_id_sensor) VALUES ('". $_POST["combo1"] ."', '". $_POST["combo2"] ."')");
			if ($query == 1)
					echo "<font color='green' size='2'>Atribuição bem sucedida. </font>";
				else
					echo "<font color='red' size='2'>Erro. Tente novamente!</font>";
		}
			


		break;

}
?>
			</div>
		</div>
	
	</div>      
</div>
