<?php
require __DIR__ . 'Database/Connection/database_connection.php';

$result = $link->query("SELECT * FROM Appointment ORDER BY date_hour ASC");
$appointments = [];

while ($row = $result->fetch_assoc()) 
{
    $appointments[] = $row;
}

echo json_encode($appointments);
$link->close();
?>