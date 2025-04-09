<?php
// Include the database connection file
require __DIR__ . '/../Connection/database_connection.php';

// Check if the request method is POST
if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    // Check if task is set and not empty
    if (isset($_POST['task']) && trim($_POST['task']) !== '') 
    {
        $task = trim($_POST['task']);
    } 
    
    else 
    {
        // Return error if task is missing
        echo json_encode(["status" => "error", "message" => "Task field is required"]);
        exit;
    }

    // Check if date_hour is set and not empty
    if (isset($_POST['date_hour']) && trim($_POST['date_hour']) !== '') 
    {
        $date_hour = trim($_POST['date_hour']);
    } 

    else 
    {
        // Return error if date_hour is missing
        echo json_encode(["status" => "error", "message" => "Date and time field is required"]);
        exit;
    }

    // Check if device_id is set and is a number
    if (isset($_POST['device_id']) && is_numeric($_POST['device_id'])) 
    {
        $device_id = intval($_POST['device_id']);
    } 

    else 
    {
        // Return error if device_id is missing or invalid
        echo json_encode(["status" => "error", "message" => "Device ID field is required and must be a number"]);
        exit;
    }

    // Double check if any field is still empty
    if (empty($task) || empty($date_hour) || empty($device_id)) 
    {
        echo json_encode(["status" => "error", "message" => "All fields are required"]);
        exit;
    }

    // Check if the device exists in the database
    $check_device = $link->prepare("SELECT device_id FROM Device WHERE device_id = ?");
    $check_device->bind_param("i", $device_id);
    $check_device->execute();
    $result = $check_device->get_result();

    if ($result->num_rows === 0) 
    {
        // Return error if device is not found
        echo json_encode(["status" => "error", "message" => "Wemos not connected"]);
        exit;
    }

    // Set default note value
    $note = "active";

    // Insert the new appointment into the database
    $stmt = $link->prepare("INSERT INTO Appointment (task, date_hour, note, device_id) VALUES (?, ?, ?, ?)");
    $stmt->bind_param("sssi", $task, $date_hour, $note, $device_id);

    if ($stmt->execute()) 
    {
        // Return success with the new appointment ID
        echo json_encode(["status" => "success", "message" => "Appointment added", "appointment_id" => $stmt->insert_id]);
    } 

    else 
    {
        // Return error if insertion failed
        echo json_encode(["status" => "error", "message" => "Error: " . $stmt->error]);
    }

    // Close statements and connection
    $stmt->close();
    $check_device->close();
    $link->close();
} 

else 
{
    // Return error if request method is not POST
    echo json_encode(["status" => "error", "message" => "Invalid request"]);
}
?>
