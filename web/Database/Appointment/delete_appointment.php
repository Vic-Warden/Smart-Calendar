<?php
// Include the database connection
require __DIR__ . '/../Connection/database_connection.php';

// Set the response content type to JSON
header('Content-Type: application/json; charset=utf-8');

// Check if the request method is DELETE
if ($_SERVER["REQUEST_METHOD"] !== "DELETE") 
{
    // Return error if method is not DELETE
    echo json_encode(["status" => "error", "message" => "Invalid request method, use DELETE"]);
    exit;
}

// Get the input data from the request body
$input_data = json_decode(file_get_contents("php://input"), true);
$appointment_id = $input_data['appointment_id'] ?? null;

// Check if appointment_id is provided and valid
if (!$appointment_id || !is_numeric($appointment_id)) 
{
    echo json_encode(["status" => "error", "message" => "Valid appointment ID is required."]);
    exit;
}

// Check if the appointment exists
$query = $link->prepare("SELECT * FROM Appointment WHERE appointment_id = ?");
$query->bind_param("i", $appointment_id);
$query->execute();
$result = $query->get_result();

if ($result->num_rows === 0) 
{
    // Return error if no appointment found
    echo json_encode(["status" => "error", "message" => "Appointment not found"]);
    exit;
}

// Fetch the appointment data before deletion
$appointment = $result->fetch_assoc();

// Update the note field before deletion
$note = "deleted";
$update_stmt = $link->prepare("UPDATE Appointment SET note = ? WHERE appointment_id = ?");
$update_stmt->bind_param("si", $note, $appointment_id);
$update_stmt->execute();
$update_stmt->close();

// Prepare and execute the delete query
$stmt = $link->prepare("DELETE FROM Appointment WHERE appointment_id = ?");
$stmt->bind_param("i", $appointment_id);

if ($stmt->execute()) 
{
    // Save the deleted appointment data into a JSON file
    file_put_contents(__DIR__ . "/last_deleted_appointment.json", json_encode($appointment, JSON_PRETTY_PRINT));

    // Return success response with deleted appointment details
    echo json_encode(["status" => "success", "message" => "Appointment deleted", "deleted_appointment" => $appointment]);
} 

else 
{
    // Return SQL error
    echo json_encode(["status" => "error", "message" => "Error: " . $stmt->error]);
}

// Close statements and database connection
$stmt->close();
$link->close();
?>
