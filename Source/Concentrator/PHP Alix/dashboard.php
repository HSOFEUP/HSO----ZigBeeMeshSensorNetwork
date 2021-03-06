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
	mysql_select_db($dbname); 
}
?>


<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>InovRetail - In Store Retail Monitoring</title>
<link href="css/site.css" rel="stylesheet" type="text/css" />


<script type="text/javascript">
	function view_node_info() {
		var store_id =  document.getElementById('view_store').value;
		if (store_id != "-")
			this.document.location.href = '?store_id=' + store_id;	
			else{ document.getElementById('tabela_r').style.visibility="hidden";
		}
	}
</script>

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

        <li><a class="active" href="dashboard.php">Visualizar</a></li>
        <li><a href="change.php">Editar</a></li>
        <li><a href="insert.php">Inserir</a></li>
        <li><a href="delete.php">Eliminar</a></li>
        <li><a href="contacto.php">Contactos</a></li>
        <li><a href="logout.php">Sair</a></li>

</div>

<div class="pesquisa">
	
	<div class="boxinfo">
	
		<div class="boxleft">
			<div class="title">Unit</div>
			<div class="details">
<?php

//obtem dados da loja
$query = mysql_query("SELECT * FROM Store LIMIT 0, 1");

while ($row = mysql_fetch_array($query)) {
	echo '<span class="fieldtitle">Nome da Loja: </span> ' . $row["name_store"] . '
				<br />
				<span class="fieldtitle">ID da Loja: </span> ' . $row["id_store"] . '
				<br />
				<span class="fieldtitle">Endereço do Web Service Remoto: </span><br />' . $row["web_service"];
}
?>
				
			</div>
			<div class="details">
				<span class="fieldtitle">Nós de Sensorização Instalados</span>
				<br /><br />
				<table border="1" background="grey">
					<tr>
						<td class="fieldtitle">ID - Nó de Senor</td>
						<td class="fieldtitle">Tempo de aquisição</td>
						<td class="fieldtitle">Descrição -  Nó de Sesor</td>
						<td class="fieldtitle">ID Sensor</td>
						<td class="fieldtitle">Descrição - Sensor</td>
					</tr>
<?php

$query = mysql_query("SELECT DISTINCT Node_id_mac, Node.time, Node.descriptor AS nodeDescriptor, Sensor_id_Sensor, Sensor.descriptor AS sensorDescriptor FROM store.Node_has_Sensor, store.Node, store.Sensor WHERE Node_id_mac = id_mac AND Sensor_id_sensor = id_sensor");
while ($row = mysql_fetch_array($query)) {
	echo '
		<tr>
			<td>' . $row["Node_id_mac"] . '</td>
			<td>' . $row["time"] . '</td>
			<td>' . $row["nodeDescriptor"] . '</td>
			<td>'. $row["Sensor_id_Sensor"] . '</td>
			<td>' . $row["sensorDescriptor"] . '</td>
		</tr>';
}
?>
				</table>
			</div>
			<div class="details">
				<span class="fieldtitle">Últimas 10 Leituras</span>
				<br /><br />
				<table border="1" background="grey">
					<tr>
						<td class="fieldtitle">IDReads</td>
						<td class="fieldtitle">Node</td>
						<td class="fieldtitle">Sensor</td>
						<td class="fieldtitle">Value 1</td>
						<td class="fieldtitle">Value 2</td>
						<td class="fieldtitle">Value 3</td>
						<td class="fieldtitle">Timestamp</td>
					</tr>
<?php


$query = mysql_query("SELECT * FROM store.Read ORDER BY timestamp DESC LIMIT 0, 10");
while ($row = mysql_fetch_array($query)) {
	echo '
		<tr>
			<td>' . $row["id_reads"] . '</td>
			<td>' . $row["id_node"] . '</td>
			<td>' . $row["id_sensor"] . '</td>
			<td>'. $row["value_1"] . '</td>
			<td>' . $row["value_2"] . '</td>
			<td>' . $row["value_3"] . '</td>
			<td>' . $row["timestamp"] . '</td>
		</tr>';
}
?>
				</table>
			</div>
			<div class="details">
				<span class="fieldtitle">Todos os Nós</span>
				<br /><br />
				<table border="1" background="grey">
					<tr>
						<td class="fieldtitle">ID MAC</td>
						<td class="fieldtitle">Time</td>
						<td class="fieldtitle">Descriptor</td>
					</tr>
<?php
$query = mysql_query("SELECT id_mac, time, descriptor FROM store.Node");
while ($row = mysql_fetch_array($query)) {
	echo '
					<tr>
						<td>' . $row["id_mac"] . '</td>
						<td>' . $row["time"] . '</td>
						<td>' . utf8_encode($row["descriptor"]) . '</td>
					</tr>';
}
?>						
				</table>
			</div>
			<div class="details">
				<span class="fieldtitle">Todos os Sensores</span>
				<br /><br />
				<table border="1" background="grey">
					<tr>
						<td class="fieldtitle">ID Sensor</td>
						<td class="fieldtitle">Descriptor</td>
					</tr>
<?php
$query = mysql_query("SELECT * FROM store.Sensor");
while ($row = mysql_fetch_array($query)) {
	echo '<tr>
			<td>' . $row["id_sensor"] . '</td>
			<td>' . utf8_encode($row["descriptor"]) . '</td>
		</tr>';
}
?>
				</table>
			</div>
			
			
			
		</div>
	
	</div>
</div>


<div class="rodape">
     <img src="images/klogo.png" name="imagem_rodape" hspace="10%" vspace="0%" id="imagem_logo"  />
</div>

</body>

</html>



