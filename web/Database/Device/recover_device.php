<?php
// Include the database connection
require __DIR__ . '/../Connection/database_connection.php';

// Set the response content type to JSON
header('Content-Type: application/json');

// Query to select all devices
$sql = "SELECT device_id, name, ip_address FROM Device";
$result = $link->query($sql);

$response = [];

// Check if there are results
if ($result && $result->num_rows > 0) 
{
    // Add each row to the response array
    while ($row = $result->fetch_assoc()) 
    {
        $response[] = $row;
    }   

    // Encode response and save to a JSON file
    $json_data = json_encode(['status' => 'success', 'devices' => $response], JSON_PRETTY_PRINT);
    file_put_contents('devices.json', $json_data);

    // Send response
    echo $json_data;
} 
else 
{
    // If no devices found
    echo json_encode(['status' => 'error', 'message' => 'No devices found.']);
}

// Close the database connection
$link->close();
?>
