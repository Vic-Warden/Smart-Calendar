<?php
// Include the database connection
require __DIR__ . '/../Connection/database_connection.php';

// Set the response content type to JSON
header('Content-Type: application/json');

$response = [];

// Check if the request method is POST
if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    // Check if sensor_id is set and valid
    if (isset($_POST['sensor_id']) && is_numeric($_POST['sensor_id'])) 
    {
        $sensor_id = intval($_POST['sensor_id']);

        // Check if the sensor exists
        $check_sensor = $link->prepare("SELECT sensor_id FROM Sensor WHERE sensor_id = ?");
        $check_sensor->bind_param("i", $sensor_id);
        $check_sensor->execute();
        $result = $check_sensor->get_result();

        if ($result->num_rows === 0) 
        {
            // Sensor does not exist
            $response['status'] = 'error';
            $response['message'] = "Sensor with ID $sensor_id does not exist.";
        } 
        else 
        {
            // Delete the sensor
            $stmt = $link->prepare("DELETE FROM Sensor WHERE sensor_id = ?");
            $stmt->bind_param("i", $sensor_id);

            if ($stmt->execute()) {
                $response['status'] = 'success';
                $response['message'] = "Sensor with ID $sensor_id successfully deleted.";
            } 
            else 
            {
                $response['status'] = 'error';
                $response['message'] = "Error while deleting the sensor.";
            }

            $stmt->close();
        }

        $check_sensor->close();
    } 
    else 
    {
        // sensor_id is missing or invalid
        $response['status'] = 'error';
        $response['message'] = "Missing or invalid sensor ID.";
    }
} 
else 
{
    // Request method is not POST
    $response['status'] = 'error';
    $response['message'] = "Invalid request method.";
}

// Return the response
echo json_encode($response);

// Close the database connection
$link->close();
?>
