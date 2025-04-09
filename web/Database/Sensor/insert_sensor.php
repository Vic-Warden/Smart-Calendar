<?php
// Include the database connection
require __DIR__ . '/../Connection/database_connection.php';

// Set the response content type to JSON
header('Content-Type: application/json');
$response = [];

// Check if the request method is POST
if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    // Get and clean input data
    $name = trim($_POST['name']);
    $type = trim($_POST['type']);
    $activation_threshold = isset($_POST['activation_threshold']) ? floatval($_POST['activation_threshold']) : null;
    $device_id = isset($_POST['device_id']) ? intval($_POST['device_id']) : 0;

    // Check required fields
    if (!empty($name) && !empty($type) && !empty($device_id)) 
    {
        // Check if sensor with same name already exists for this device
        $stmt = $link->prepare("SELECT sensor_id FROM Sensor WHERE name = ? AND device_id = ?");
        $stmt->bind_param("si", $name, $device_id);
        $stmt->execute();
        $result = $stmt->get_result();

        if ($result->num_rows > 0) 
        {
            // Sensor already exists
            $response['status'] = 'error';
            $response['message'] = 'A sensor with this name already exists for this device.';
        } 
        else 
        {
            // Insert new sensor
            $stmt = $link->prepare("INSERT INTO Sensor (name, type, activation_threshold, device_id) VALUES (?, ?, ?, ?)");
            $stmt->bind_param("ssdi", $name, $type, $activation_threshold, $device_id);

            if ($stmt->execute()) 
            {
                $response['status'] = 'success';
                $response['message'] = 'Sensor added successfully.';
                $response['sensor'] = [
                    'sensor_id' => $stmt->insert_id,
                    'name' => $name,
                    'type' => $type,
                    'activation_threshold' => $activation_threshold,
                    'device_id' => $device_id
                ];
            } 
            else 
            {
                $response['status'] = 'error';
                $response['message'] = $stmt->error;
            }
        }

        $stmt->close();
    } 
    else 
    {
        // Required fields missing
        $response['status'] = 'error';
        $response['message'] = 'Please fill in all required fields.';
    }

    // Return JSON response
    echo json_encode($response);
}

// Close database connection
$link->close();
?>
