<?php 
include "config.php";

$st = new DDStat();
$res = $st->initTrackServer($_SC['trk_host'], $_SC['trk_port']);
var_dump($res);

$res = $st->trackByUDP(1, 512341234, "nihao");
var_dump($res);

?>
