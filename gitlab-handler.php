<?php

// Задача: написать обработчик входящих json объектов, который складывает их в файловую систему

ini_set('display_errors', true);

define('ATOM', 'Y-m-d\TH:i:s');
define('TOKEN_HEADER', 'HTTP_X_GITLAB_TOKEN');
define('HOOKS_DIR', 'hooks/');

$token = 'secret';

if (!isset($_SERVER[TOKEN_HEADER])) {
	log_error(TOKEN_HEADER . ' is not set');
}

if (isset($_SERVER[TOKEN_HEADER]) && $token != $_SERVER[TOKEN_HEADER]) {
	log_error('token is wrong, received: ' . $_SERVER[TOKEN_HEADER]);
}

// dump received json
$json = file_get_contents('php://input');
$hook = json_decode($json, true);
$txt = print_r($hook, true);
$fname = HOOKS_DIR . date(ATOM) . '_' . $hook['object_kind'];

file_put_contents($fname . '.json', $json);
file_put_contents($fname . '.txt', $txt);


function log_error(string $message)
{
	$logfile = 'gitlab-handler.log';
	$message = date(ATOM) . ' '. $message . "\n";
	file_put_contents($logfile, $message, FILE_APPEND | LOCK_EX);
}
