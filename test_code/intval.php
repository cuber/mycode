<?php
$a = "123\n";
$fp = fopen("./a.txt", "r+");
var_dump(intval(trim(fgets($fp))));
