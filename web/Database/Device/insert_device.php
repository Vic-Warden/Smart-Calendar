<?php
require_once __DIR__ . '/../Connection/database_connection.php';

header('Content-Type: application/json');

if ($_SERVER["REQUEST_METHOD"] !== "POST") 
{
    http_response_code(405);
    echo json_encode(["error" => "Méthode non autorisée. Utilisez POST."]);
    exit;
}

if (!isset($_POST['name']) || !isset($_POST['ip_address'])) 
{
    http_response_code(400);
    echo json_encode(["error" => "Paramètres manquants."]);
    exit;
}

$name = trim($_POST['name']);
$ip_address = trim($_POST['ip_address']);

if (empty($name) || empty($ip_address)) 
{
    http_response_code(400);
    echo json_encode(["error" => "Le nom et l'adresse IP ne peuvent pas être vides."]);
    exit;
}

$name = mysqli_real_escape_string($link, htmlspecialchars($name));
$ip_address = mysqli_real_escape_string($link, htmlspecialchars($ip_address));

if (!filter_var($ip_address, FILTER_VALIDATE_IP)) 
{
    http_response_code(400);
    echo json_encode(["error" => "Adresse IP invalide."]);
    exit;
}

$check_query = "SELECT device_id, name, ip_address FROM Device WHERE name = ?";
$stmt_check = mysqli_prepare($link, $check_query);
mysqli_stmt_bind_param($stmt_check, "s", $name);
mysqli_stmt_execute($stmt_check);
$result = mysqli_stmt_get_result($stmt_check);

if ($row = mysqli_fetch_assoc($result)) 
{
    $device_id = $row['device_id'];
    $old_ip = $row['ip_address'];

    if ($old_ip !== $ip_address) 
    {
        $update_query = "UPDATE Device SET ip_address = ? WHERE device_id = ?";
        $stmt_update = mysqli_prepare($link, $update_query);
        mysqli_stmt_bind_param($stmt_update, "si", $ip_address, $device_id);

        if (mysqli_stmt_execute($stmt_update)) 
        {
            http_response_code(200);
            echo json_encode([
                "success" => "Adresse IP mise à jour.",
                "device" => [
                    "device_id" => $device_id,
                    "name" => $name,
                    "ip_address" => $ip_address
                ]
            ]);
        } 

        else 
        {
            http_response_code(500);
            echo json_encode(["error" => "Erreur lors de la mise à jour de l'adresse IP."]);
        }

        mysqli_stmt_close($stmt_update);
    } 

    else 
    {
        http_response_code(200);
        echo json_encode([
            "success" => "Aucune modification nécessaire.",
            "device" => 
            [
                "device_id" => $device_id,
                "name" => $name,
                "ip_address" => $ip_address
            ]
        ]);
    }
} 

else 
{
    $insert_query = "INSERT INTO Device (name, ip_address) VALUES (?, ?)";
    $stmt_insert = mysqli_prepare($link, $insert_query);
    mysqli_stmt_bind_param($stmt_insert, "ss", $name, $ip_address);

    if (mysqli_stmt_execute($stmt_insert)) 
    {
        $new_device_id = mysqli_insert_id($link);
        http_response_code(201);
        echo json_encode([
            "success" => "Appareil ajouté avec succès!",
            "device" => 
            [
                "device_id" => $new_device_id,
                "name" => $name,
                "ip_address" => $ip_address
            ]
        ]);
    } 
    else 
    {
        http_response_code(500);
        echo json_encode(["error" => "Erreur lors de l'insertion."]);
    }
    
    mysqli_stmt_close($stmt_insert);
}

mysqli_stmt_close($stmt_check);
mysqli_close($link);
?>