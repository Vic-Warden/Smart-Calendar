<?php
// Include the database connection
require __DIR__ . '/../Connection/database_connection.php';

// Set the response content type to JSON
header('Content-Type: application/json');

$response = [];

// Check if request method is GET
if ($_SERVER["REQUEST_METHOD"] == "GET") 
{
    // Prepare a query to get the last 20 data entries across all sensors
    $stmt = $link->prepare("
        SELECT sensor_id, value, time_stamp 
        FROM SensorData 
        WHERE data_id IN (
            SELECT data_id 
            FROM (SELECT data_id FROM SensorData ORDER BY time_stamp DESC LIMIT 20) AS temp
        )
        ORDER BY sensor_id, time_stamp DESC
    ");

    if ($stmt->execute()) 
    {
        $result = $stmt->get_result();
        $sensorData = [];

        // Group data by sensor_id
        while ($row = $result->fetch_assoc()) 
        {
            $sensorData[$row['sensor_id']][] = 
            [
                "value" => $row['value'],
                "timestamp" => $row['time_stamp']
            ];
        }

        // Build success response
        $response['status'] = 'success';
        $response['sensors'] = $sensorData;
    } 
    else 
    {
        // SQL error
        $response['status'] = 'error';
        $response['message'] = 'Error while retrieving sensor data.';
    }

    $stmt->close();
} 
else 
{
    // Invalid method
    $response['status'] = 'error';
    $response['message'] = 'Invalid method.';
}

// Send the response
echo json_encode($response);
$link->close();
?>
