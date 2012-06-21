<?php

$pid = pcntl_fork();
$_SGLOBAL = array();
//父进程和子进程都会执行下面代码
if ($pid == -1) {
	//错误处理：创建子进程失败时返回-1.
	die('could not fork');
} else if ($pid) {
	//父进程会得到子进程号，所以这里是父进程执行的逻辑
	//pcntl_wait($status); //等待子进程中断，防止子进程成为僵尸进程。
	exit;
} else {
	//子进程得到的$pid为0, 所以这里是子进程执行的逻辑。
	echo posix_getpid()."\n";
}

for($i = 0; $i < 3; $i ++) {
	$fork_pid = pcntl_fork();
	if($fork_pid) {
		$_SGLOBAL['pid'][] = $fork_pid;
		continue;
	} else {
		sleep(100);
		exit;
	}
}

function stop_all()
{
	global $_SGLOBAL;
	foreach($_SGLOBAL['pid'] AS $pid) {
		posix_kill($pid, 9);
		var_dump($pid." killed");
	}
}

pcntl_signal(SIGTERM, "sig_handler");
pcntl_signal(SIGHUP,  "sig_handler");
pcntl_signal(SIGUSR1, "sig_handler");

//信号处理函数
function sig_handler($signo)
{

	switch ($signo) {
	case SIGTERM:
		var_dump("trigger sigterm");
		// 处理SIGTERM信号
		stop_all();
		exit("SIGTERM signal caught\n");
		break;
	case SIGHUP:
		//处理SIGHUP信号
		echo "SIGHUP signal caught\n";
		break;
	case SIGUSR1:
		echo "Caught SIGUSR1...\n";
		break;
	default:
		// 处理所有其他信号
	}

}

echo "Installing signal handler...\n";

//pcntl_signal_dispatch();

while(1) {
	echo "before dispatch\n";
	sleep(10);
	pcntl_signal_dispatch();
	echo "after dispatch\n";
	//sleep(30);
}
