--TEST--
Check DDStat
--SKIPIF--
<?php if (!extension_loaded("dd")) print "skip"; ?>
--FILE--
<?php 
$st = new DDStat();
$res = $st->initTrackServer("127.0.0.1", 8260);
var_dump($res);

?>
--EXPECT--
bool(true)
