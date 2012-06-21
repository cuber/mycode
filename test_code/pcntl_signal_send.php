<?php
posix_kill(965, SIGTERM);
var_dump(pcntl_signal_dispatch());
