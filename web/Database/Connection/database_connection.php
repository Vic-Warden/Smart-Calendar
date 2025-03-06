<?php
$host = 'mariadb';
$user = 'root';
$password = '7YKyE8R2AhKzswfN';
$database = 'iot';

$link = mysqli_connect($host, $user, $password, $database);

if (!$link) 
{
    die('Emperor forgive me, I have failed: ' . mysqli_connect_error());
}

$server_ip = gethostbyname(gethostname());

$data = 
[
    "server_ip" => $server_ip,
    "timestamp" => date("Y-m-d H:i:s")
];

file_put_contents(__DIR__ . "/server_ip.json", json_encode($data, JSON_PRETTY_PRINT));
?>
