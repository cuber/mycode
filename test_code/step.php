<?php
$msg = "<99080000007d00000a0000000a0000002e0c00002e0c0000>,<98080000187900000a0000000a0000002b0b00002b0b0000>,<40060000204e00000a0000000a0000001c0800001c080000>,<b0040000c83200000a0000000a0000001206000012060000>,<20030000102700000a0000000a0000000b8800000b880000>";

$res = array();
foreach(explode(",", $msg) AS $key => $value) {
	$value = trim($value, "<>");
	preg_match_all("#\w{8}#", $value, $tmp);
	foreach($tmp[0] AS $_k => $_v) {
		preg_match_all("#\w{2}#", $_v, $tmp_sub);
		$tmp_reverse = array_reverse($tmp_sub[0]);
		$tmp_all[$_k] = implode("", $tmp_reverse);
	}
	$res[$key] = implode(" ", $tmp_all);
}
var_dump($res);
