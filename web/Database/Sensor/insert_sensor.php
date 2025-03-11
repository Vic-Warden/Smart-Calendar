<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');
$response = [];

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    $name = trim($_POST['name'] ?? '');
    $type = trim($_POST['type'] ?? '');
    $activation_threshold = isset($_POST['activation_threshold']) ? floatval($_POST['activation_threshold']) : null;
    $device_id = isset($_POST['device_id']) ? intval($_POST['device_id']) : 0;

    if (empty($name) || empty($type) || empty($device_id)) 
    {
        echo json_encode(["status" => "error", "message" => "Tous les champs requis doivent être remplis."]);
        exit;
    }

    $valid_types = ['Presence', 'Light', 'Button'];
    if (!in_array($type, $valid_types)) 
    {
        echo json_encode(["status" => "error", "message" => "Type de capteur invalide. Types valides : Presence, Light, Button."]);
        exit;
    }

    $stmt = $link->prepare("SELECT device_id FROM Device WHERE device_id = ?");
    $stmt->bind_param("i", $device_id);
    $stmt->execute();
    $result = $stmt->get_result();

    if ($result->num_rows === 0) 
    {
        echo json_encode(["status" => "error", "message" => "L'appareil (device_id) spécifié n'existe pas."]);
        exit;
    }

    $stmt = $link->prepare("INSERT INTO Sensor (name, type, activation_threshold, device_id) VALUES (?, ?, ?, ?)");
    $stmt->bind_param("ssdi", $name, $type, $activation_threshold, $device_id);

    if ($stmt->execute()) 
    {
        $response['status'] = 'success';
        $response['message'] = 'Capteur ajouté avec succès.';
        $response['sensor'] = [
            'sensor_id' => $stmt->insert_id,
            'name' => $name,
            'type' => $type,
            'activation_threshold' => $activation_threshold,
            'device_id' => $device_id
        ];
    } 
    
    else 
    {
        $response['status'] = 'error';
        $response['message'] = $stmt->error;
    }

    $stmt->close();
    echo json_encode($response);
}

$link->close();
?>
