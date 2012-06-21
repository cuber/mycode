<?php

$pid = pcntl_fork();
if ($pid == -1) {
	die("could not fork");
}
if ($pid != 0) {
	exit;
}


$fp_lock = fopen("./flock.lock", "r+");
if(flock($fp_lock, LOCK_EX | LOCK_NB)) {
	fwrite($fp_lock, posix_getpid());
} else {
	echo fgets($fp_lock)."\n";
	exit("already running exit automatically \n");
}

sleep(100);
