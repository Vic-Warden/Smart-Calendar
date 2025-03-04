<?php
header("Content-Type: application/json");

$json_file = __DIR__ . "/last_deleted_appointment.json";

if (!file_exists($json_file)) 
{
    echo json_encode(["status" => "error", "message" => "No deleted appointment found"]);
} 

else 
{
    echo file_get_contents($json_file);
}
?>
