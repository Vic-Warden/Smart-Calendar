<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');
$response = [];

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    $sensor_id = isset($_POST['sensor_id']) ? intval($_POST['sensor_id']) : 0;
    $button_state = isset($_POST['button_state']) ? intval($_POST['button_state']) : 0;
    $time_stamp = date('Y-m-d H:i:s'); 

    if (!empty($sensor_id)) 
    {
        $stmt = $link->prepare("INSERT INTO SensorData (value, time_stamp, button_state, sensor_id) VALUES (?, ?, ?, ?)");
        $stmt->bind_param("dsii", $button_state, $time_stamp, $button_state, $sensor_id);

        if ($stmt->execute()) 
        {
            $delete_stmt = $link->prepare("
                DELETE FROM SensorData 
                WHERE sensor_id = ? 
                AND data_id NOT IN (
                    SELECT data_id 
                    FROM (SELECT data_id FROM SensorData WHERE sensor_id = ? ORDER BY time_stamp DESC LIMIT 10) AS temp_table
                )
            ");
            $delete_stmt->bind_param("ii", $sensor_id, $sensor_id);
            $delete_stmt->execute();
            $delete_stmt->close();

            $response['status'] = 'success';
            $response['message'] = 'Donnée du bouton enregistrée et gestion des 10 dernières valeurs effectuée.';
            $response['data'] = [
                'data_id' => $stmt->insert_id,
                'sensor_id' => $sensor_id,
                'button_state' => $button_state,
                'time_stamp' => $time_stamp
            ];
        } 

        else 
        {
            $response['status'] = 'error';
            $response['message'] = $stmt->error;
        }

        $stmt->close();
    } 

    else 
    {
        $response['status'] = 'error';
        $response['message'] = 'ID du capteur manquant.';
    }

    echo json_encode($response);
}
$link->close();
?>
