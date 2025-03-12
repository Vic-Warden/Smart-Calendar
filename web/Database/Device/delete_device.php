<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');

$response = [];

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    if (isset($_POST['device_id']) && is_numeric($_POST['device_id'])) 
    {
        $device_id = intval($_POST['device_id']);

        $check_device = $link->prepare("SELECT device_id FROM Device WHERE device_id = ?");
        $check_device->bind_param("i", $device_id);
        $check_device->execute();
        $result = $check_device->get_result();

        if ($result->num_rows === 0) 
        {
            $response['status'] = 'error';
            $response['message'] = "L'appareil avec l'ID $device_id n'existe pas.";
        } 
        
        else 
        {
            $stmt = $link->prepare("DELETE FROM Device WHERE device_id = ?");
            $stmt->bind_param("i", $device_id);

            if ($stmt->execute()) 
            {
                $response['status'] = 'success';
                $response['message'] = "Appareil avec l'ID $device_id supprimé avec succès.";
            } 
            
            else 
            {
                $response['status'] = 'error';
                $response['message'] = "Erreur lors de la suppression de l'appareil.";
            }

            $stmt->close();
        }

        $check_device->close();
    } 
    
    else 
    {
        $response['status'] = 'error';
        $response['message'] = "ID d'appareil manquant ou invalide.";
    }
} 

else 
{
    $response['status'] = 'error';
    $response['message'] = "Méthode de requête invalide.";
}

echo json_encode($response);
$link->close();
?>
