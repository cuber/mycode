<?php
//$a = array('b' => 1); unset($a['c']);
$a = array("a" => 1, "b" => 2);
foreach($a AS $k => &$v) {
	if($k == "b") {
	}
};
var_dump($a);
unset($v);
//$v = 5;
var_dump($a);
