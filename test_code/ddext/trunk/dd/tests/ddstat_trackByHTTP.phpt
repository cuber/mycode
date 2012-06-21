--TEST--
Check DDStat
--SKIPIF--
<?php if (!extension_loaded("dd")) print "skip"; ?>
--FILE--
<?php 
include "config.php";

$st = new DDStat();
$res = $st->initTrackServer($_SC['trk_host'], $_SC['trk_port']);
var_dump($res);

$res = $st->trackByHTTP(1, 512341234, "nihao");
var_dump($res);

?>
--EXPECT--
bool(true)
bool(true)
