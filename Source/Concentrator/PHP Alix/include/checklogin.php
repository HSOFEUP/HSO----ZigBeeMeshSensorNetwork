<?php

session_start();

if (isset($_SESSION["username"]))
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
	$query = mysql_query("SELECT * FROM store.User WHERE username = '" . $_SESSION["username"] . "' LIMIT 0, 1");

	$total_rows = mysql_num_rows($query);
	
	//close the connection
	mysql_close($con);

	if ($total_rows==0 || !$total_rows) {
		header("Location: $notlogged");
	}
	else if (isset($force_logged_users)){
		header("Location: $loggedpage");
	}
}
else if (!isset($force_logged_users) || (isset($force_logged_users) && $force_logged_users != true)) {
	header("Location: $notlogged");
}
?>
