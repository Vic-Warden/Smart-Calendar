<?php
echo "hhh";

$mysqli = new mysqli("mariadb","root","7YKyE8R2AhKzswfN","iot");

if ($mysqli -> connect_errno) {
    echo "Failed to connect to MySQL: " . $mysqli -> connect_error;
    exit();
  }
?>