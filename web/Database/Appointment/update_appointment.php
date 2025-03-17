<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json; charset=utf-8');

if ($_SERVER["REQUEST_METHOD"] !== "PUT") 
{
    echo json_encode(["status" => "error", "message" => "Méthode de requête invalide, utilisez PUT"]);
    exit;
}

$input_data = json_decode(file_get_contents("php://input"), true);

$appointment_id = $input_data['appointment_id'] ?? null;
$task = $input_data['task'] ?? null;
$date_hour = $input_data['date_hour'] ?? null;

if (!$appointment_id || !$task || !$date_hour) 
{
    echo json_encode(["status" => "error", "message" => "Tous les champs sont requis."]);
    exit;
}

$note = "updated";
$stmt = $link->prepare("UPDATE Appointment SET task = ?, date_hour = ?, note = ? WHERE appointment_id = ?");
$stmt->bind_param("sssi", $task, $date_hour, $note, $appointment_id);

if ($stmt->execute()) 
{
    $query = $link->prepare("SELECT * FROM Appointment WHERE appointment_id = ?");
    $query->bind_param("i", $appointment_id);
    $query->execute();
    $result = $query->get_result();
    $appointment = $result->fetch_assoc();

    file_put_contents(__DIR__ . "/last_updated_appointment.json", json_encode($appointment, JSON_PRETTY_PRINT));

    echo json_encode([
        "status" => "success",
        "message" => "Appointment updated",
        "updated_appointment" => $appointment
    ]);
} 

else 
{
    echo json_encode(["status" => "error", "message" => "Erreur lors de la mise à jour : " . $stmt->error]);
}

$stmt->close();
$link->close();
?>
