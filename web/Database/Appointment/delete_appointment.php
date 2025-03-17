<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json; charset=utf-8');

if ($_SERVER["REQUEST_METHOD"] !== "DELETE") 
{
    echo json_encode(["status" => "error", "message" => "Méthode de requête invalide, utilisez DELETE"]);
    exit;
}

$input_data = json_decode(file_get_contents("php://input"), true);
$appointment_id = $input_data['appointment_id'] ?? null;

if (!$appointment_id || !is_numeric($appointment_id)) 
{
    echo json_encode(["status" => "error", "message" => "Valid appointment ID is required."]);
    exit;
}

$query = $link->prepare("SELECT * FROM Appointment WHERE appointment_id = ?");
$query->bind_param("i", $appointment_id);
$query->execute();
$result = $query->get_result();

if ($result->num_rows === 0) 
{
    echo json_encode(["status" => "error", "message" => "Appointment not found"]);
    exit;
}

$appointment = $result->fetch_assoc();

$note = "deleted";
$update_stmt = $link->prepare("UPDATE Appointment SET note = ? WHERE appointment_id = ?");
$update_stmt->bind_param("si", $note, $appointment_id);
$update_stmt->execute();
$update_stmt->close();

$stmt = $link->prepare("DELETE FROM Appointment WHERE appointment_id = ?");
$stmt->bind_param("i", $appointment_id);

if ($stmt->execute()) 
{
    file_put_contents(__DIR__ . "/last_deleted_appointment.json", json_encode($appointment, JSON_PRETTY_PRINT));

    echo json_encode(["status" => "success", "message" => "Appointment deleted", "deleted_appointment" => $appointment]);
} 

else 
{
    echo json_encode(["status" => "error", "message" => "Error: " . $stmt->error]);
}

$stmt->close();
$link->close();
?>
