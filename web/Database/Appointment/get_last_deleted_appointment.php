<?php
// Set the response content type to JSON
header("Content-Type: application/json");

// Path to the JSON file containing the last deleted appointment
$json_file = __DIR__ . "/last_deleted_appointment.json";

// Check if the file exists
if (!file_exists($json_file)) 
{
    // Return error if the file does not exist
    echo json_encode(["status" => "error", "message" => "No deleted appointment found"]);
} 

else 
{
    // Return the content of the JSON file
    echo file_get_contents($json_file);
}
?>
