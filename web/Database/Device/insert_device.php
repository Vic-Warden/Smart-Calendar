<?php
// Include the database connection
require __DIR__ . '/../Connection/database_connection.php';

// Set the response content type to JSON
header('Content-Type: application/json');
$response = [];

// Check if the request method is POST
if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    // Get and trim input values
    $name = trim($_POST['name']);
    $ip_address = trim($_POST['ip_address']);

    // Check if both fields are provided
    if (!empty($name) && !empty($ip_address)) 
    {
        // Check if a device with the same name already exists
        $stmt = $link->prepare("SELECT device_id FROM Device WHERE name = ?");
        $stmt->bind_param("s", $name);
        $stmt->execute();
        $result = $stmt->get_result();

        if ($result->num_rows > 0) 
        {
            // Device exists, update its IP address
            $row = $result->fetch_assoc();
            $device_id = $row['device_id'];

            $stmt = $link->prepare("UPDATE Device SET ip_address = ? WHERE device_id = ?");
            $stmt->bind_param("si", $ip_address, $device_id);
            
            if ($stmt->execute()) 
            {
                $response['status'] = 'success';
                $response['message'] = 'IP address updated.';
                $response['device'] = [
                    'device_id' => $device_id,
                    'name' => $name,
                    'ip_address' => $ip_address
                ];
            } 
            else 
            {
                $response['status'] = 'error';
                $response['message'] = $stmt->error;
            }
        } 
        else 
        {
            // Device does not exist, insert new one
            $stmt = $link->prepare("INSERT INTO Device (name, ip_address) VALUES (?, ?)");
            $stmt->bind_param("ss", $name, $ip_address);

            if ($stmt->execute()) 
            {
                $response['status'] = 'success';
                $response['message'] = 'New device added successfully.';
                $response['device'] = [
                    'device_id' => $stmt->insert_id,
                    'name' => $name,
                    'ip_address' => $ip_address
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
        // If name or IP is missing
        $response['status'] = 'error';
        $response['message'] = 'Please fill in all required fields.';
    }

    // Send the JSON response
    echo json_encode($response);
}

// Close the database connection
$link->close();
?>
