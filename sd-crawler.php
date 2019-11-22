<?php

// Задача: разработать демон для постоянной выгрузки данных через апи,
// который будет нагружать ее (service desk) запросами равномерно

define('SD_HOST', 'https://...');
define('SD_USER', 'sa-gitlab-crawler:password');
define('WAITING_RC_STATUS', 52);

define('SCAN_INTERVAL', 30);
define('TASK_INTERVAL', 2);

define('ATOM', 'Y-m-d\TH:i:s');
define('TASKS_DIR', 'tasks/');


$last_scan = 0;
$jobs = array();

while (true) {
	scan();
	if (count($jobs)) {
		dump_task(array_shift($jobs));
	}
}


function scan()
{
	global $last_scan, $jobs;
	$now = time();

	if ($now - $last_scan < SCAN_INTERVAL) {
		return false;
	}

	$last_scan = $now;
	log_message('downloading tasks');
	$json = get_tasks();
	sleep(TASK_INTERVAL);

	$data = json_decode($json, true);
	if (isset($data['Tasks'])) {
		$jobs = array_merge($data['Tasks'], $jobs);
	}

	log_message('preparing rc task list');
	$json = get_rc_tasks();
	file_put_contents('rc_tasks.json', $json);
	sleep(TASK_INTERVAL);
}


function dump_task($task)
{
	$name = implode('_', array(
		$task['Id'], $task['Changed'], status_name($task['StatusId']),
	));

	$fname = TASKS_DIR . $name . '.json';
	if (file_exists($fname)) {
		// log_message($fname . ' exists');
		return false;
	}

	log_message('new ' . $fname);
	$json = get_one_task($task['Id']);
	file_put_contents($fname, $json);
	sleep(TASK_INTERVAL);
}


function status_name(int $status)
{
	switch ($status) {
	case 52:
		return 'waitrc';
	case 53:
		return 'devtest';
	case 27:
		return 'inwork';
	default:
		return 'unknown';
	}
}


function log_message(string $message)
{
	$logfile = 'sd-crawler.log';
	$message = date(ATOM) . ' ' . $message . "\n";
	file_put_contents($logfile, $message, FILE_APPEND | LOCK_EX);
}

// // $json = get_tasks();
// $json = get_rc_tasks();
// // $json = get_one_task(332432);
// // $json = curl_auth_send(SD_HOST . '/api/filter?resource=task', SD_USER);
// $data = json_decode($json, true);
// print_r($data);


function get_tasks()
{
	$url = SD_HOST . '/api/task?filterid=2814&fields=Id,StatusId,Changed';
	return curl_auth_send($url, SD_USER);
}


function get_rc_tasks()
{
	$url = SD_HOST . '/api/task?filterid=2814&fields=Id,StatusId&pagesize=100';
	$json = curl_auth_send($url, SD_USER);

	$data = json_decode($json, true);
	$tasks = array();

	foreach ($data['Tasks'] as $task) {
		if (WAITING_RC_STATUS === $task['StatusId']) {
			$tasks[] = $task['Id'];
		}
	}

	return json_encode($tasks);
}


function get_one_task(int $taskid)
{
	$url = SD_HOST . '/api/task/' . $taskid;
	return curl_auth_send($url, SD_USER);
}


function get_task_view(int $taskid)
{
	$url = SD_HOST . '/task/view/' . $taskid;
	return curl_auth_send($url, SD_USER);
}


function change_task(int $taskid, array $changes)
{
	$url = SD_HOST . '/api/task/' . $taskid;
	$headers[] = 'Content-Type: application/json';

	$options = array(
		CURLOPT_CUSTOMREQUEST => 'PUT',
		CURLOPT_USERPWD => SD_USER,
		CURLOPT_HTTPAUTH => CURLAUTH_NTLM,
		CURLOPT_HTTPHEADER => $headers,
		CURLOPT_POSTFIELDS => json_encode($changes),
	);

	return curl_send($url, $options);
}


function curl_send(string $url, array $curl_options = null)
{
	// http://php.net/manual/en/function.curl-setopt.php
	$options = array(
		CURLOPT_RETURNTRANSFER => true, // curl_exec returns transfer as a string
		CURLOPT_FOLLOWLOCATION => true,
		CURLOPT_MAXREDIRS => 3,
		CURLOPT_TIMEOUT => 10,

		// CURLOPT_HEADER => true,
		// CURLOPT_VERBOSE => true,
		// CURLOPT_STDERR => fopen('php://stdout', 'w'),
		// CURLINFO_HEADER_OUT => true,
	);

	foreach ($curl_options as $flag => $value) {
		$options[$flag] = $value;
	}


	$resource = curl_init($url);
	// see http://php.net/manual/en/function.curl-setopt-array.php
	curl_setopt_array($resource, $options);
	// http://php.net/manual/en/function.curl-setopt.php#110457
	// curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, false); // !!

	$response = curl_exec($resource);
	// echo curl_getinfo($resource, CURLINFO_TOTAL_TIME) . "s\n"; // DEBUG
	curl_close($resource);
	return $response;
}


/*	Passing an array to CURLOPT_POSTFIELDS will encode the data as multipart/form-data,
	while passing a URL-encoded string will encode the data as application/x-www-form-urlencoded. */
function curl_post(string $url, $post_fields)
{
	$options = array(
		CURLOPT_POST => true,
		CURLOPT_POSTFIELDS => $post_fields,
	);

	return curl_send($url, $options);
}


function curl_post_json(string $url, array $data)
{
	$headers[] = 'Content-Type: application/json';

	$options = array(
		CURLOPT_POST => true,
		CURLOPT_HTTPHEADER => $headers,
		CURLOPT_POSTFIELDS => json_encode($data),
		// CURLOPT_HTTP_VERSION => CURL_HTTP_VERSION_1_1,
		// CURLOPT_ENCODING => '', // 'Accept-Encoding: ' header contains all supported encodings
	);

	return curl_send($url, $options);
}


function curl_auth_send(string $url, string $credentials)
{
	$options = array(
		CURLOPT_HTTPAUTH => CURLAUTH_BASIC,
		CURLOPT_USERPWD => $credentials,
	);

	return curl_send($url, $options);
}
