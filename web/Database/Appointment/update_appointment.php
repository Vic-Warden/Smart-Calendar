<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json; charset=utf-8');

if ($_SERVER["REQUEST_METHOD"] !== "PUT") 
{
    echo json_encode(["status" => "error", "message" => "Invalid request method, use PUT"]);
    exit;
}

$input_data = json_decode(file_get_contents("php://input"), true);

file_put_contents("log_update.txt", json_encode($input_data, JSON_PRETTY_PRINT));

$appointment_id = $input_data['appointment_id'] ?? null;
$task = $input_data['task'] ?? null;
$date_hour = $input_data['date_hour'] ?? null;

if (!$appointment_id || !$task || !$date_hour) 
{
    echo json_encode(["status" => "error", "message" => "All fields are required"]);
    exit;
}

$note = "updated";

$stmt = $link->prepare("UPDATE Appointment SET task = ?, date_hour = ?, note = ? WHERE appointment_id = ?");
$stmt->bind_param("sssi", $task, $date_hour, $note, $appointment_id);

if ($stmt->execute()) 
{
    echo json_encode(["status" => "success", "message" => "Appointment updated"]);
} 

else 
{
    echo json_encode(["status" => "error", "message" => "Erreur SQL : " . $stmt->error]);
}

$stmt->close();
$link->close();
?>

