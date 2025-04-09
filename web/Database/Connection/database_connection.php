<?php
// Database connection settings
$host = 'mariadb';
$user = 'root';
$password = '7YKyE8R2AhKzswfN';
$database = 'iot';

// Create the connection
$link = mysqli_connect($host, $user, $password, $database);

// Check if the connection failed
if (!$link) 
{
    // If it fails, stop the script and display an error message
    die('Emperor forgive me, I have failed' . mysqli_connect_error());
}
?>
