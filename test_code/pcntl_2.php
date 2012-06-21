<?php

$_SGLOBAL = array();


job::start(3);


class job {
	
	public static function sig_handle($signal) {
		global $_SGLOBAL;
		switch ($signal) {
			case SIGTERM:
				var_dump($_SGLOBAL['pid']);
				// sigterm signal
				//self::_signal_all_stop();
				break;
			// all other signal
			default:
				break;
		}
	}
	
	public static function start($count) {
		global $_SGLOBAL;
		self::run_as_daemon();
		pcntl_signal(SIGTERM, array('job', 'sig_handle'));
		for ($i = 0; $i < $count; $i++) {
			$fork_pid = pcntl_fork();
			if($fork_pid) {
				$_SGLOBAL['pid'][] = $fork_pid;
				continue;
			}
			sleep(100);
		}
		while (1) {
			pcntl_signal_dispatch();
			sleep(100);
		}
	}
	
	public static function run_as_daemon() {
		$fork_pid = pcntl_fork();
		if($fork_pid) {
			exit;
		}
	}
} 