<?php
// Include the database connection
require __DIR__ . '/../Connection/database_connection.php';

// Set the response content type to JSON
header('Content-Type: application/json');

$response = [];

// Check if the request method is POST
if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    // Check if device_id is provided and valid
    if (isset($_POST['device_id']) && is_numeric($_POST['device_id'])) 
    {
        $device_id = intval($_POST['device_id']);

        // Check if the device exists in the database
        $check_device = $link->prepare("SELECT device_id FROM Device WHERE device_id = ?");
        $check_device->bind_param("i", $device_id);
        $check_device->execute();
        $result = $check_device->get_result();

        if ($result->num_rows === 0) 
        {
            // Device does not exist
            $response['status'] = 'error';
            $response['message'] = "Device with ID $device_id does not exist.";
        } 
        
        else 
        {
            // Device exists, proceed to delete
            $stmt = $link->prepare("DELETE FROM Device WHERE device_id = ?");
            $stmt->bind_param("i", $device_id);

            if ($stmt->execute()) 
            {
                $response['status'] = 'success';
                $response['message'] = "Device with ID $device_id successfully deleted.";
            } 
            else 
            {
                $response['status'] = 'error';
                $response['message'] = "Error while deleting the device.";
            }

            $stmt->close();
        }

        $check_device->close();
    } 
    else 
    {
        // Invalid or missing device_id
        $response['status'] = 'error';
        $response['message'] = "Missing or invalid device ID.";
    }
} 
else 
{
    // Invalid request method
    $response['status'] = 'error';
    $response['message'] = "Invalid request method.";
}

// Return the JSON response
echo json_encode($response);

// Close the database connection
$link->close();
?>
