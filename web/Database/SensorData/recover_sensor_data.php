<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');

$response = [];

if ($_SERVER["REQUEST_METHOD"] == "GET") 
{
    $stmt = $link->prepare("
        SELECT sensor_id, value, time_stamp 
        FROM SensorData 
        WHERE data_id IN (
            SELECT data_id 
            FROM (SELECT data_id FROM SensorData ORDER BY time_stamp DESC LIMIT 20) AS temp
        )
        ORDER BY sensor_id, time_stamp DESC
    ");

    if ($stmt->execute()) 
    {
        $result = $stmt->get_result();
        $sensorData = [];

        while ($row = $result->fetch_assoc()) 
        {
            $sensorData[$row['sensor_id']][] = 
            [
                "value" => $row['value'],
                "timestamp" => $row['time_stamp']
            ];
        }

        $response['status'] = 'success';
        $response['sensors'] = $sensorData;
    } 

    else 
    {
        $response['status'] = 'error';
        $response['message'] = 'Erreur lors de la récupération des données.';
    }

    $stmt->close();
} 

else 
{
    $response['status'] = 'error';
    $response['message'] = 'Méthode invalide.';
}

echo json_encode($response);
$link->close();
?>
