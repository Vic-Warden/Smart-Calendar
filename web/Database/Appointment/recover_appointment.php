<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json; charset=utf-8');

$result = $link->query("SELECT * FROM Appointment");

$appointments = [];

while ($row = $result->fetch_assoc()) 
{
    $appointments[] = $row;  //
}

echo json_encode($appointments, JSON_PRETTY_PRINT);

$link->close();
?>
