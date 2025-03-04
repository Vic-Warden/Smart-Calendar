<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json; charset=utf-8');

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{

    $appointment_id = $_POST['appointment_id'] ?? null;
    $task = $_POST['task'] ?? null;
    $date_hour = $_POST['date_hour'] ?? null;

    if (!$appointment_id || !$task || !$date_hour) 
    {
        echo json_encode(["status" => "error", "message" => "Tous les champs sont requis."]);
        exit;
    }

    $stmt = $link->prepare("UPDATE Appointment SET task = ?, date_hour = ? WHERE appointment_id = ?");
    $stmt->bind_param("ssi", $task, $date_hour, $appointment_id);

    if ($stmt->execute()) 
    {
        echo json_encode(["status" => "success", "message" => "Rendez-vous mis à jour avec succès !"]);
    } 
    
    else 
    {
        echo json_encode(["status" => "error", "message" => "Erreur lors de la mise à jour : " . $stmt->error]);
    }

    $stmt->close();
    $link->close();
} 

else 
{
    echo json_encode(["status" => "error", "message" => "Méthode de requête invalide"]);
}
?>
