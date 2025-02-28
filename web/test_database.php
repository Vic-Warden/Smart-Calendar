<?php
$servername = "localhost";
$username = "root"; //
$password = "7YKyE8R2AhKzswfN";
$dbname = "iot";


$login = new mysqli($servername, $username, $password, $dbname);


if ($login->connect_error) {
    die("❌ Connection failed: " . $login->connect_error);
} else {
    echo "✅ Connection successful!";
}

$login->close();
?>
