<?php
$servername = "localhost";
$username = "root";
$password = "7YKyE8R2AhKzswfN";
$dbname = "iot";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("❌" . $conn->connect_error);
} else {
    echo "✅";
}

$conn->close();
?>
