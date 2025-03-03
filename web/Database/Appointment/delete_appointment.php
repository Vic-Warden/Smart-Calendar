<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json; charset=utf-8');

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $appointment_id = $_POST['appointment_id'] ?? null;

    if (!$appointment_id || !is_numeric($appointment_id)) {
        echo json_encode(["status" => "error", "message" => "Valid appointment ID is required."]);
        exit;
    }

    $stmt = $link->prepare("DELETE FROM Appointment WHERE appointment_id = ?");
    $stmt->bind_param("i", $appointment_id);

    if ($stmt->execute()) {
        echo json_encode(["status" => "success", "message" => "Appointment deleted successfully."]);
    } else {
        echo json_encode(["status" => "error", "message" => "Error: " . $stmt->error]);
    }

    $stmt->close();
    $link->close();
} else {
    echo json_encode(["status" => "error", "message" => "Invalid request method."]);
}
?>
