--TEST--
Check DDStat
--SKIPIF--
<?php if (!extension_loaded("dd")) print "skip"; ?>
--FILE--
<?php 
$st = new DDStat();
var_dump($st);

?>
--EXPECT--
object(DDStat)#1 (0) {
}
