<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');
$response = [];

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    $name = trim($_POST['name']);
    $type = trim($_POST['type']);
    $activation_threshold = isset($_POST['activation_threshold']) ? floatval($_POST['activation_threshold']) : null;
    $device_id = isset($_POST['device_id']) ? intval($_POST['device_id']) : 0;

    if (!empty($name) && !empty($type) && !empty($device_id)) 
    {
        $stmt = $link->prepare("SELECT sensor_id FROM Sensor WHERE name = ? AND device_id = ?");
        $stmt->bind_param("si", $name, $device_id);
        $stmt->execute();
        $result = $stmt->get_result();

        if ($result->num_rows > 0) 
        {
            $response['status'] = 'error';
            $response['message'] = 'Un capteur avec ce nom existe déjà pour ce périphérique.';
        } 

        else 
        {
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
        }

        $stmt->close();
    } 
    else 
    {
        $response['status'] = 'error';
        $response['message'] = 'Veuillez remplir tous les champs requis.';
    }

    echo json_encode($response);
}

$link->close();
?>
