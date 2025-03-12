<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');

$response = [];

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    if (isset($_POST['sensor_id']) && is_numeric($_POST['sensor_id'])) 
    {
        $sensor_id = intval($_POST['sensor_id']);

        $check_sensor = $link->prepare("SELECT sensor_id FROM Sensor WHERE sensor_id = ?");
        $check_sensor->bind_param("i", $sensor_id);
        $check_sensor->execute();
        $result = $check_sensor->get_result();

        if ($result->num_rows === 0) 
        {
            $response['status'] = 'error';
            $response['message'] = "Le capteur avec l'ID $sensor_id n'existe pas.";
        } 
        
        else 
        {
            $stmt = $link->prepare("DELETE FROM Sensor WHERE sensor_id = ?");
            $stmt->bind_param("i", $sensor_id);

            if ($stmt->execute()) {
                $response['status'] = 'success';
                $response['message'] = "Capteur avec l'ID $sensor_id supprimé avec succès.";
            } 
            
            else 
            {
                $response['status'] = 'error';
                $response['message'] = "Erreur lors de la suppression du capteur.";
            }

            $stmt->close();
        }

        $check_sensor->close();
    } 
    
    else 
    {
        $response['status'] = 'error';
        $response['message'] = "ID du capteur manquant ou invalide.";
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
