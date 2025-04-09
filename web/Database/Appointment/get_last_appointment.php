<?php
// Set the response content type to JSON
header("Content-Type: application/json");

// Define the path to the JSON file
$json_file = __DIR__ . "/last_appointment.json";

// Check if the JSON file exists
if (file_exists($json_file)) 
{
    // If it exists, return its contents
    echo file_get_contents($json_file);
} 

else 
{
    // If not, return an error message in JSON
    echo json_encode(["status" => "error", "message" => "No appointment found"]);
}
?>
