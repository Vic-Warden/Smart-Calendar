<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');

$response = [];

if ($_SERVER["REQUEST_METHOD"] == "GET") {
    $stmt = $link->prepare("SELECT sensor_id, name, type, activation_threshold, device_id FROM Sensor");
    $stmt->execute();
    $result = $stmt->get_result();

    if ($result->num_rows > 0) 
    {
        while ($row = $result->fetch_assoc()) 
        {
            $response[] = [
                "sensor_id" => $row["sensor_id"],
                "name" => $row["name"],
                "type" => $row["type"],
                "activation_threshold" => $row["activation_threshold"],
                "device_id" => $row["device_id"]
            ];
        }
        echo json_encode(["status" => "success", "sensors" => $response]);
    } 
    
    else 
    {
        echo json_encode(["status" => "error", "message" => "Aucun capteur trouvé."]);
    }

    $stmt->close();
} 

else 
{
    echo json_encode(["status" => "error", "message" => "Requête invalide."]);
}

$link->close();
?>
