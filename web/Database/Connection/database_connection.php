<?php
$host = 'mariadb';
$user = 'root';
$password = '7YKyE8R2AhKzswfN';
$database = 'iot';

$link = mysqli_connect($host, $user, $password, $database);

if (!$link) 
{
    die('Emporor forgive me, I have failed' . mysqli_connect_error());
}

echo 'Retreat is not an option !';

mysqli_close($link);
?>