<?php
touch("./a.pid");
$fp = fopen("./a.pid", "r+");
fwrite($fp, posix_getpid());
