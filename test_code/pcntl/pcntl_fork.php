<?php

// run as daemon
$fork_pid = pcntl_fork();
// father get the pid number of child
// child  get 0
if($fork_pid > 0) {
	exit; // detach from shell
} 
echo "father's pid: ".posix_getpid()."\n";
// start child loop
job('start');
// listen the child process
for(;1;) {
	$dead_child_pid = pcntl_wait($wait);
	echo "child process: $dead_child_pid was dead, exit signal: $wait\n";
	// restart child process
	job('restart');
	sleep(1);
}
// job
function job($type) {
	$fork_pid = pcntl_fork();
	if($fork_pid > 0) { return; }
	echo "job $type pid: ".posix_getpid()."\n";
	for(;1;) { sleep(1); }
}
