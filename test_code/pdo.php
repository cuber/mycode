<?php
try {
	//$db = new PDO("dblib:dbname=ddscm;host=10.255.255.119;port=14330;charset=GB18030", "readuser", "password");
	$db = new PDO("dblib:host=10.255.255.119:14330;dbname=ddscm", "writeuser", "ddbackend");
	var_dump($db->query("1+1"));
} catch (PDOException $e) {
	var_dump($e->getMessage());
}

