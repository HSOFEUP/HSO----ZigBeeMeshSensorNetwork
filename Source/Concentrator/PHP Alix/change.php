<?php
require_once ("include/config.php");
require_once ("include/checklogin.php");

$result = false;
$message = "";

$node_data = "";
$node_name = "";
$node_type = "";

if (1) {

	//establish the connection
	$con = mysql_connect($dbhost, $dbuser, $dbpassword);

	if (!$con) {
		die('Could not connect: ' . mysql_error());
	}

	//select database
	mysql_select_db($dbname, $con);
}
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
        <li><a class="active" href="change.php">Editar</a></li>
        <li><a href="insert.php">Inserir</a></li>
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
				<a href="?option=1"<?php if (isset($_GET["option"]) && $_GET["option"] == 1) echo ' class="active"'; ?>>Editar Nó</a>
				<a href="?option=2"<?php if (isset($_GET["option"]) && $_GET["option"] == 2) echo ' class="active"'; ?>>Editar Sensor</a>
				<a href="?option=3"<?php if (isset($_GET["option"]) && $_GET["option"] == 3) echo ' class="active"'; ?>>Editar Loja</a>
				<a href="?option=4"<?php if (isset($_GET["option"]) && $_GET["option"] == 4) echo ' class="active"'; ?>>Editar Utilizador</a>
			</div>
			<div class="detail_options">
<?php


switch ($_GET["option"]) 
{
	case 1 :
		//nós
		if (!isset ($_GET["macId"])) 
		{
			$query = mysql_query("SELECT id_mac FROM store.Node LIMIT 0, 1");
			while ($row = mysql_fetch_array($query)) 
			{
				$macId = $row["id_mac"];
			}
		} 
		else 
		{
			$macId = $_GET["macId"];
		}
?>
					<form name="node_edition" id="node_edition" method="post" action="?option=1&action=1&macId=<?php echo $macId; ?>">
						<br />
						<script type="text/javascript">
							function updateNode(id_mac) 
							{
								window.location.href = '?option=1&macId=' + id_mac;
							}
						</script>
						<strong>Seleccione um nó de sensorização:</strong><br /><br />
						<select name="id_mac" id="id_mac" onchange="updateNode(this.value);">
<?php

		$query = mysql_query("SELECT id_mac FROM store.Node");
		while ($row = mysql_fetch_array($query)) 
		{
			echo '
											<option value="' . $row["id_mac"] . '"';
			if (isset ($_GET["macId"]) && $_GET["macId"] == $row["id_mac"])
				echo ' selected';
			echo '>' . $row["id_mac"] . '</option>
												';
		}


$query = mysql_query("SELECT * FROM store.Node WHERE id_mac = '" . $macId . "' LIMIT 0, 1");
while ($row = mysql_fetch_array($query)) {
	echo '</select>
												<br /><br />
												Tempo de Aquisição: <input type="text" name="timestamp" value="' . $row["time"] . '" />
												<br />Descrição do Nó: <input type="text" name="descriptor" value="' . utf8_encode($row["descriptor"]) . '" />
												<br /><br />';
}
?>
						<input type="submit" name="submitbt" value="Actualizar" />
						<br /><br />
					</form>
					
					
<?php


			if (isset ($_GET["action"])) 
			{
				//foi pedido para actualizar a BD
				$query = mysql_query("UPDATE store.Node SET time = '" . $_POST["timestamp"] . "', status_read = -1, descriptor = '" . $_POST["descriptor"] . "' WHERE id_mac = '" . $_POST["id_mac"] . "'");
				echo $_POST["timestamp"];
				echo $_POST["descriptor"];
				echo $_POST["id_mac"];
			}
		break;

case 2 :
	//sensores
	if (!isset ($_GET["sensId"])) 
	{
		$query = mysql_query("SELECT id_sensor FROM store.Sensor LIMIT 0, 1");
		while ($row = mysql_fetch_array($query)) 
		{
			$sensId = $row["id_sensor"];
		}
	} 
	else 
	{
		$sensId = $_GET["sensId"];
	}
?>
					<form name="sensor_edition" id="sensor_edition" method="post" action="?option=2&action=2&sensId=<?php echo $sensId; ?>">
						<br />
						<script type="text/javascript">
							function updateSensor(id_sensor) 
							{
								window.location.href = '?option=2&sensId=' + id_sensor;
							}
						</script>
						<strong>Seleccione um sensor:</strong><br /><br />
						<select name="id_sensor" id="id_sensor" onchange="updateSensor(this.value);">
<?php


	$query = mysql_query("SELECT id_sensor FROM store.Sensor");
	while ($row = mysql_fetch_array($query)) 
	{
		echo '
										<option value="' . $row["id_sensor"] . '"';
		if (isset ($_GET["sensId"]) && $_GET["sensId"] == $row["id_sensor"])
			echo ' selected';
		echo '>' . $row["id_sensor"] . '</option>
											';
	}

	$query = mysql_query("SELECT descriptor FROM store.Sensor WHERE id_sensor = '" . $sensId . "' LIMIT 0, 1");
	while ($row = mysql_fetch_array($query)) 
	{
		echo '</select>
													<br /><br />
													Descrição do Nó: <input type="text" name="descriptor" value="' . utf8_encode($row["descriptor"]) . '" />
													<br /><br />';
	}
?>
						<input type="submit" name="submitbt" value="Actualizar" />
						<br /><br />
					</form>
					
<?php


	if (isset ($_GET["action"])) 
	{
		//foi pedido para actualizar a BD
		$query = mysql_query("UPDATE store.Sensor SET descriptor = '" . $_POST["descriptor"] . "' WHERE id_sensor = '" . $_POST["id_sensor"] . "'");
	}

	break;

case 3 :
	//lojas
?>		
		<form method="post" action="">
<?php

	$query = mysql_query("SELECT * FROM store.Store LIMIT 0, 1");
	while ($row = mysql_fetch_array($query)) 
	{
		$id_store = utf8_encode($row["id_store"]);
		$name_store = utf8_encode($row["name_store"]);
		$web_service = utf8_encode($row["web_service"]);
	}

	echo '</select>
					<br />
					ID da loja: <input type="text" name="id_store" value="' . $id_store . '" />
					<br /><br />';
	echo '</select>
					Nome da loja: <input type="text" name="name_store" value="' . $name_store . '" />
					<br /><br />';

	echo '</select>
					Endereço Web Service Remoto: <input type="text" name="web_service" size="70" value="' . $web_service . '" />
					<br /><br />';
?>
						<input type="submit" name="submitt" value="Actualizar" />
						<br /><br />
					</form>
					
<?php


	if (isset ($_POST["submitt"])) 
	{
		//foi pedido para actualizar a BD
		$query = mysql_query("UPDATE store.Store SET id_store = '" . $_POST["id_store"] . "', name_store = '" . $_POST["name_store"] . "', web_service = '" . $_POST["web_service"] . "' WHERE 1");
	}
	break;
case 4 :
	//utilizadores
?>		
		<form method="post" action="">
<?php

	$query = mysql_query("SELECT * FROM store.User WHERE username = '" . $_SESSION["username"] . "' LIMIT 0, 1");
	while ($row = mysql_fetch_array($query)) 
	{
		$username = utf8_encode($row["username"]);
		$email = utf8_encode($row["email"]);
	}

	echo '</select> <br />
					Email: <input type="text" name="email" size="50" value="' . $email . '" />';
?>
				<input type="submit" name="submit1" value="Actualizar" />
						<br /><br /><br />
<?php


	echo '</select>
					Nova Passoword: <input type="password" name="new_password1" />';
	echo '</select>
					Repita a Passoword: <input type="password" name="new_password2" />';
?>
						<input type="submit" name="submit2" value="Actualizar" />
						<br /><br />
					</form>
<?php


	if (isset ($_POST["submit1"])) 
	{
		//foi pedido para actualizar a BD
		$query = mysql_query("UPDATE store.User SET email = '" . $_POST["email"] . "' WHERE username = '" . $_SESSION["username"] . "' ");
	}

	if (isset ($_POST["submit2"]) && ($_POST["new_password1"] != $_POST["new_password2"])) 
	{
		echo "<font color='red' size='2'>Passwords não coincidem! Por favor introduza a password nos dois campos corretamente.</font>";
	} 
	if (isset ($_POST["submit2"]) && ($_POST["new_password1"] == $_POST["new_password2"])) 
	{
			$query = mysql_query("UPDATE store.User SET password = '" . $_POST["new_password1"] . "' WHERE username = '" . $_SESSION["username"] . "' ");
			echo "<font color='green' size='2'>Password alterada corretamente. </font>";
			//session_start();
			//session_destroy();
			//header("Location: $initialpage");
			//exit;
	}

	break;

}
?>
			</div>
		</div>
	
	</div>      
</div>
