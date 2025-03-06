<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');
$response = [];

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    $name = trim($_POST['name']);
    $ip_address = trim($_POST['ip_address']);

    if (!empty($name) && !empty($ip_address)) 
    {
        $stmt = $link->prepare("INSERT INTO Device (name, ip_address) VALUES (?, ?)");
        $stmt->bind_param("ss", $name, $ip_address);

        if ($stmt->execute()) 
        {
            $response['status'] = 'success';
            $response['message'] = 'Nouveau périphérique ajouté avec succès.';
            $response['device'] = 
            [
                'device_id' => $stmt->insert_id,
                'name' => $name,
                'ip_address' => $ip_address
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
        $response['message'] = 'Veuillez remplir tous les champs requis.';
    }

    echo json_encode($response);
}

$link->close();
?>
