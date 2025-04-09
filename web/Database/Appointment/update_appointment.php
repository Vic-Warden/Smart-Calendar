<?php
// Include the database connection
require __DIR__ . '/../Connection/database_connection.php';

// Set the response content type to JSON
header('Content-Type: application/json; charset=utf-8');

// Check if the request method is PUT
if ($_SERVER["REQUEST_METHOD"] !== "PUT") 
{
    // Return error if the request method is not PUT
    echo json_encode(["status" => "error", "message" => "Invalid request method, use PUT"]);
    exit;
}

// Get the JSON input from the request body
$input_data = json_decode(file_get_contents("php://input"), true);

// Save the input to a log file (for debugging)
file_put_contents("log_update.txt", json_encode($input_data, JSON_PRETTY_PRINT));

// Get values from the input
$appointment_id = $input_data['appointment_id'] ?? null;
$task = $input_data['task'] ?? null;
$date_hour = $input_data['date_hour'] ?? null;

// Check if all fields are present
if (!$appointment_id || !$task || !$date_hour) 
{
    echo json_encode(["status" => "error", "message" => "All fields are required"]);
    exit;
}

// Set the note value
$note = "updated";

// Prepare the SQL query to update the appointment
$stmt = $link->prepare("UPDATE Appointment SET task = ?, date_hour = ?, note = ? WHERE appointment_id = ?");
$stmt->bind_param("sssi", $task, $date_hour, $note, $appointment_id);

// Execute the query and return success or error
if ($stmt->execute()) 
{
    echo json_encode(["status" => "success", "message" => "Appointment updated"]);
} 

else 
{
    echo json_encode(["status" => "error", "message" => "Erreur SQL : " . $stmt->error]);
}

// Close the statement and the database connection
$stmt->close();
$link->close();
?>
