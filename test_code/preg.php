<?php
$charset = "utf-8";
$charset = strtoupper(preg_replace("#[^a-z0-9]#i", "", $charset));
var_dump($charset);
