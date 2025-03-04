<?php
require __DIR__ . '/../Connection/database_connection.php';

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    if (isset($_POST['task']) && trim($_POST['task']) !== '') 
    {
        $task = trim($_POST['task']);
    } 
    else 
    {
        echo json_encode(["status" => "error", "message" => "Task field is required"]);
        exit;
    }

    if (isset($_POST['date_hour']) && trim($_POST['date_hour']) !== '') 
    {
        $date_hour = trim($_POST['date_hour']);
    } 
    else 
    {
        echo json_encode(["status" => "error", "message" => "Date and time field is required"]);
        exit;
    }

    if (isset($_POST['device_id']) && is_numeric($_POST['device_id'])) 
    {
        $device_id = intval($_POST['device_id']);
    } 
    else 
    {
        echo json_encode(["status" => "error", "message" => "Device ID field is required and must be a number"]);
        exit;
    }

    if (empty($task) || empty($date_hour) || empty($device_id)) 
    {
        echo json_encode(["status" => "error", "message" => "All fields are required"]);
        exit;
    }

    // Vérifier si le device existe
    $check_device = $link->prepare("SELECT device_id FROM Device WHERE device_id = ?");
    $check_device->bind_param("i", $device_id);
    $check_device->execute();
    $result = $check_device->get_result();

    if ($result->num_rows === 0) 
    {
        echo json_encode(["status" => "error", "message" => "Wemos not connected"]);
        exit;
    }

    $stmt = $link->prepare("INSERT INTO Appointment (task, date_hour, device_id) VALUES (?, ?, ?)");
    $stmt->bind_param("ssi", $task, $date_hour, $device_id);

    if ($stmt->execute()) 
    {
        $last_id = $stmt->insert_id;

        $query = $link->prepare("SELECT * FROM Appointment WHERE appointment_id = ?");
        $query->bind_param("i", $last_id);
        $query->execute();
        $result = $query->get_result();
        $appointment = $result->fetch_assoc();

        file_put_contents(__DIR__ . "/last_appointment.json", json_encode($appointment, JSON_PRETTY_PRINT));

        echo json_encode(["status" => "success", "message" => "Appointment added", "appointment" => $appointment]);
    } 
    else 
    {
        echo json_encode(["status" => "error", "message" => "Error: " . $stmt->error]);
    }

    $stmt->close();
    $check_device->close();
    $link->close();
} 
else 
{
    echo json_encode(["status" => "error", "message" => "Invalid request"]);
}
?>
