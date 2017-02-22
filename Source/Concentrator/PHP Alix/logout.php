<?php
require_once("include/config.php");
session_start();
session_destroy();
header("Location: $initialpage");
exit;
?>

<body>
<div>Clicar</div>
</body>