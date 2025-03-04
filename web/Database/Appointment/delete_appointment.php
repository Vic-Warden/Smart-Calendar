<?php
require __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json; charset=utf-8');

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    $appointment_id = $_POST['appointment_id'] ?? null;

    if (!$appointment_id || !is_numeric($appointment_id)) 
    {
        echo json_encode(["status" => "error", "message" => "Valid appointment ID is required."]);
        exit;
    }

    $stmt = $link->prepare("DELETE FROM Appointment WHERE appointment_id = ?");
    $stmt->bind_param("i", $appointment_id);

    if ($stmt->execute()) 
    {
        $json_file = __DIR__ . "/last_appointment.json";
        if (file_exists($json_file)) 
        {
            $last_appointment = json_decode(file_get_contents($json_file), true);
            if ($last_appointment && $last_appointment["appointment_id"] == $appointment_id) 
            {
                $query = $link->query("SELECT * FROM Appointment ORDER BY appointment_id DESC LIMIT 1");
                $new_last_appointment = $query->fetch_assoc();
    
                if ($new_last_appointment) 
                {
                    file_put_contents($json_file, json_encode($new_last_appointment, JSON_PRETTY_PRINT));
                } 
                
                else 
                {
                    file_put_contents($json_file, json_encode(["status" => "error", "message" => "No appointment found"], JSON_PRETTY_PRINT));
                }
            }
        }
        echo json_encode(["status" => "success", "message" => "Appointment deleted"]);
    }
    
    
    else 
    {
        echo json_encode(["status" => "error", "message" => "Error: " . $stmt->error]);
    }

    $stmt->close();
    $link->close();
} 

else 
{
    echo json_encode(["status" => "error", "message" => "Invalid request method."]);
}
?>
