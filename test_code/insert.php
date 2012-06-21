<?php
$fp = fopen("/Users/Cube/Documents/DangDang/direct_mail/2012-04-10/list.txt", "r");

$sql = <<<SQL
insert 
    into dm_mail_queue
        (status, msg_to, es_event_id, event_id, creation_date, last_modified_date)
    values
        (5, ?, 12, ?, ?, ?)
SQL;
$db = new PDO("dblib:host=10.255.255.113:1433;dbname=Msg_queue", "writeuser", "ddbackend");

$sth = $db->prepare($sql);

$item = ""; $event_id = 0; $date = date("Y-m-d H:i:s");

$sth->bindParam(1, $item, PDO::PARAM_STR);
$sth->bindParam(2, $event_id, PDO::PARAM_INT);
$sth->bindParam(3, $date, PDO::PARAM_STR);
$sth->bindParam(4, $date, PDO::PARAM_STR);

$count = 0;

while(!feof($fp)) {
	$item = trim(fgets($fp));
	$event_id = mt_rand(0, 1) ? 21000 : 22000;
	$date = date("Y-m-d H:i:s");
	$res = $sth->execute();
	var_dump($count++);
}
