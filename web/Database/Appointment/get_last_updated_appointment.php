<?php
// Set the response content type to JSON
header("Content-Type: application/json");

// Path to the last updated appointment JSON file
$json_file = __DIR__ . "/last_updated_appointment.json";

// Check if the file exists
if (file_exists($json_file)) 
{
    // If it exists, return its content
    echo file_get_contents($json_file);
} 

else 
{
    // If not, return an error message
    echo json_encode(["status" => "error", "message" => "No updated appointment found"]);
}
?>
