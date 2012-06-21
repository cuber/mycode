<?php
foreach(array("info", "notice", "error", "pid") AS $level) {
	$level = str_pad(strtoupper(), 6, " ", STR_PAD_BOTH);
}
$level = str_pad("ERROR", 5, " ", STR_PAD_BOTH);

