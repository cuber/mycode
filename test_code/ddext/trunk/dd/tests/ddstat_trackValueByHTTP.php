<?php 
include "config.php";

$st = new DDStat();
$res = $st->initTrackServer($_SC['trk_host'], $_SC['trk_port']);
var_dump($res);

$res = $st->trackValueByHTTP(1, 512341234, 123);
var_dump($res);

$res = $st->trackValueByHTTP(1, 512341234, "132");
var_dump($res);
?>
