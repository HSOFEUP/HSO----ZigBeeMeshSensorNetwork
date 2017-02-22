<?php 
$dbhost = "localhost";
$dbname = "store";
$dbuser = "root";
$dbpassword = "voyage";

echo "TENTA ENVIAR DADOS PARA O WEB SERVICE" . "<br />";

$store_id = $_GET["store_id"];
$node_id = $_GET["node_id"];
$sensor_id = $_GET["sensor_id"];
$read_datetime = $_GET["read_datetime"];
$read_value_lux = $_GET["read_value_lux"];
$read_value_visible_light = $_GET["read_value_visible_light"];
$read_value_infra_red_light = $_GET["read_value_infra_red_light"];
$read_value_dba = $_GET["read_value_dba"];
$web_service = $_GET["web_service"];
	
echo "Variables: <br />";
echo $store_id . "<br />";
echo $node_id . "<br />";
echo $sensor_id . "<br />";
echo $read_datetime . "<br />";
echo $read_value_lux . "<br />";
echo $read_value_visible_light . "<br />";
echo $read_value_infra_red_light . "<br />";
echo $read_value_dba . "<br />";
echo $web_service . "<br />";
echo "<br />";

try{ 
    $client = new SoapClient($web_service); 

    $params = array('store_id' => $store_id, 
    				'node_id' => $node_id, 
    				'sensor_id' => $sensor_id, 
    				'read_datetime' => $read_datetime, 
    				'read_value_lux' => $read_value_lux, 
    				'read_value_visible_light' => $read_value_visible_light,
    				'read_value_infra_red_light' => $read_value_infra_red_light, 
    				'read_value_dba' => $read_value_dba);
    				 
    //$webService = $client->GetData($params); 
    //$wsResult = $webService->GetDataResult; 
	
	var_dump($client->__getFunctions());
	$result = $client->savereading($params);
	
	$allOK = $result->savereadingResult;
	echo "Result: " . $allOK;
	
	if ($allOK != 0) //Cannot send data to Web Service
	{
		//establish the connection
		$con = mysql_connect($dbhost, $dbuser, $dbpassword);
		if (!$con)
		{
		die('Could not connect: ' . mysql_error());
		}
		//select database
		mysql_select_db($dbname, $con);
		//run query
		if ($sensor_id==1)
		{
			$query = mysql_query("INSERT INTO store.Read 
					(id_reads, id_node, id_sensor, value_1, value_2, value_3, timestamp) VALUES 
					(NULL, '". $node_id ."', '". $sensor_id ."', '". $read_value_lux ."', 
					'". $read_value_visible_light ."', '".$read_value_infra_red_light ."', CURRENT_TIMESTAMP)");
		
			if ($query==1)
			echo "Inserido na BD Dados da Luz";
		}
		else
		{
			$query = mysql_query("INSERT INTO store.Read 
					(id_reads, id_node, id_sensor, value_1, timestamp) VALUES 
					(NULL, '". $node_id ."', '". $sensor_id ."', '". $read_value_dba ."', CURRENT_TIMESTAMP)");
			if ($query==1)
				echo "Inserido na BD Dados do Som";
		}
		mysql_close($con);
	}
}	 
catch (Exception $e) 
{
    echo  'Caught exception: '.  $e->getMessage(). "\n"; 
   		//establish the connection
		$con = mysql_connect($dbhost, $dbuser, $dbpassword);
		if (!$con)
		{
		die('Could not connect: ' . mysql_error());
		}
		//select database
		mysql_select_db($dbname, $con);
		//run query
		if ($sensor_id==1)
		{
			$query = mysql_query("INSERT INTO store.Read 
					(id_reads, id_node, id_sensor, value_1, value_2, value_3, timestamp) VALUES 
					(NULL, '". $node_id ."', '". $sensor_id ."', '". $read_value_lux ."', 
					'". $read_value_visible_light ."', '".$read_value_infra_red_light ."', CURRENT_TIMESTAMP)");
		
			if ($query==1)
			echo "Inserido na BD Luz";
		}
		else
		{
			$query = mysql_query("INSERT INTO store.Read
					(id_reads, id_node, id_sensor, value_1, timestamp) VALUES 
					(NULL, '". $node_id ."', '". $sensor_id ."', '". $read_value_dba ."', CURRENT_TIMESTAMP)");
			if ($query==1)
			echo "Inserido na BD Som";
		}
		mysql_close($con);
}
?> 
