<?php
// Include the database connection
require __DIR__ . '/../Connection/database_connection.php';

// Set the response content type to JSON
header('Content-Type: application/json; charset=utf-8');

// Execute a query to get all appointments
$result = $link->query("SELECT * FROM Appointment");

// Create an array to store the results
$appointments = [];

// Loop through the results and add them to the array
while ($row = $result->fetch_assoc()) 
{
    $appointments[] = $row;
}

// Return all appointments as a JSON array
echo json_encode($appointments, JSON_PRETTY_PRINT);

// Close the database connection
$link->close();
?>
