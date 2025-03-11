<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');

$response = [];

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    if (isset($_POST['sensor_id_1'], $_POST['value_1'], $_POST['sensor_id_2'], $_POST['value_2'], $_POST['sensor_id_3'], $_POST['value_3'])) 
    {
        $sensor_id_1 = intval($_POST['sensor_id_1']);
        $value_1 = floatval($_POST['value_1']);
        
        $sensor_id_2 = intval($_POST['sensor_id_2']);
        $value_2 = floatval($_POST['value_2']);
        
        $sensor_id_3 = intval($_POST['sensor_id_3']);
        $value_3 = floatval($_POST['value_3']);

        $timestamp = date('Y-m-d H:i:s');

        $stmt = $link->prepare("INSERT INTO SensorData (value, time_stamp, sensor_id) VALUES (?, ?, ?)");

        $stmt->bind_param("dsi", $value_1, $timestamp, $sensor_id_1);
        $stmt->execute();

        $stmt->bind_param("dsi", $value_2, $timestamp, $sensor_id_2);
        $stmt->execute();

        $stmt->bind_param("dsi", $value_3, $timestamp, $sensor_id_3);
        $stmt->execute();

        $stmt->close();

        $stmtDelete = $link->prepare("
            DELETE FROM SensorData 
            WHERE sensor_id = ? 
            AND data_id NOT IN (
                SELECT data_id FROM (
                    SELECT data_id FROM SensorData WHERE sensor_id = ? ORDER BY time_stamp DESC LIMIT 20
                ) AS temp
            )");

        foreach ([$sensor_id_1, $sensor_id_2, $sensor_id_3] as $sensor_id) 
        {
            $stmtDelete->bind_param("ii", $sensor_id, $sensor_id);
            $stmtDelete->execute();
        }
        
        $stmtDelete->close();

        $response['status'] = 'success';
        $response['message'] = 'Données insérées avec succès et anciennes données supprimées.';
        $response['data'] = [
            ["sensor_id" => $sensor_id_1, "value" => $value_1, "timestamp" => $timestamp],
            ["sensor_id" => $sensor_id_2, "value" => $value_2, "timestamp" => $timestamp],
            ["sensor_id" => $sensor_id_3, "value" => $value_3, "timestamp" => $timestamp]
        ];
    } 
    
    else 
    {
        $response['status'] = 'error';
        $response['message'] = 'Données manquantes.';
    }
} 

else 
{
    $response['status'] = 'error';
    $response['message'] = 'Requête invalide.';
}

echo json_encode($response);
$link->close();
?>
