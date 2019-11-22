<?php

// Задача: написать минимальную обвязку для демонизации скриптов

// https://www.php.net/manual/en/control-structures.declare.php#123676
// https://blog.pascal-martin.fr/post/php71-en-other-new-things/
declare(ticks=1);


$dying = ' functions is not available';

if (!function_exists('pcntl_fork')) die('PCNTL' . $dying);
if (!function_exists('posix_setsid')) die('POSIX' . $dying);


function daemonize()
{
	$pid = pcntl_fork();
	if ($pid == -1)
		die('could not fork');
	if ($pid > 0)
		exit; // parent process

	if (posix_setsid() == -1)
		die('could not detach from terminal');

	pcntl_signal(SIGHUP, 'sig_handler');
	pcntl_signal(SIGINT, 'sig_handler');
	pcntl_signal(SIGTERM, 'sig_handler');
}

function sig_handler($signo)
{
	switch ($signo) {
	case SIGHUP:
		// everything is ok, continue
		break;
	case SIGINT:
	case SIGKILL:
	case SIGTERM:
		exit;
	}
}
