<?php
header("Content-Type: application/json");

$json_file = __DIR__ . "/last_appointment.json";

if (file_exists($json_file)) 
{
    echo file_get_contents($json_file);
} 

else 
{
    echo json_encode(["status" => "error", "message" => "No appointment found"]);
}
?>