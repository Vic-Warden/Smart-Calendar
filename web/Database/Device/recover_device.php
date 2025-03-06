<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');

$sql = "SELECT device_id, name, ip_address FROM Device";
$result = $link->query($sql);

$response = [];

if ($result && $result->num_rows > 0) 
{
    while ($row = $result->fetch_assoc()) 
    {
        $response[] = $row;
    }

    $json_data = json_encode(['status' => 'success', 'devices' => $response], JSON_PRETTY_PRINT);
    file_put_contents('devices.json', $json_data);

    echo $json_data;
} 

else 
{
    echo json_encode(['status' => 'error', 'message' => 'Aucun périphérique trouvé.']);
}

$link->close();
?>
