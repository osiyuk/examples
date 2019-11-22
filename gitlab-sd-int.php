<?php

// Задача: обеспечить синхронизацию между запусками важных питон скриптов

define('INTERVAL', 3);
define('SCAN', 'python3.7 app/scripts/sdscan.py');
define('HANDLE', 'python3.7 app/scripts/hookshandler.py');


chdir('v1');
exec('export PYTHONPATH=.');

while (true) {
	echo exec(SCAN);
	sleep(INTERVAL);
	echo exec(HANDLE);
	sleep(INTERVAL);
}
