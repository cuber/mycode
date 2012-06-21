<?php
$fp_lock = fopen("./flock.lock", "r+");
if(flock($fp_lock, LOCK_EX | LOCK_NB)) {
    //fwrite($fp_lock, posix_getpid());
	echo "locked \n";
} else {
    echo fgets($fp_lock)."\n";
    exit("already running exit automatically \n");
}

