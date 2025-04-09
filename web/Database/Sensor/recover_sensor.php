<?php
// Include the database connection
require __DIR__ . '/../Connection/database_connection.php';

// Set the response content type to JSON
header('Content-Type: application/json');

$response = [];

// Check if the request method is GET
if ($_SERVER["REQUEST_METHOD"] == "GET") 
{
    // Prepare the query to get all sensors
    $stmt = $link->prepare("SELECT sensor_id, name, type, activation_threshold, device_id FROM Sensor");
    $stmt->execute();
    $result = $stmt->get_result();

    if ($result->num_rows > 0) 
    {
        // Fetch each sensor and add it to the response
        while ($row = $result->fetch_assoc()) 
        {
            $response[] = [
                "sensor_id" => $row["sensor_id"],
                "name" => $row["name"],
                "type" => $row["type"],
                "activation_threshold" => $row["activation_threshold"],
                "device_id" => $row["device_id"]
            ];
        }

        // Return the list of sensors
        echo json_encode(["status" => "success", "sensors" => $response]);
    } 

    else 
    {
        // No sensors found
        echo json_encode(["status" => "error", "message" => "No sensors found."]);
    }

    $stmt->close();
} 

else 
{
    // Invalid request method
    echo json_encode(["status" => "error", "message" => "Invalid request."]);
}

// Close the database connection
$link->close();
?>
