<?php
$host = 'localhost';
$user = 'root';
$password = '7YKyE8R2AhKzswfN';
$database = 'iot';

$link = mysqli_connect($host, $user, $password, $database);

if (!$link) {
    die('Échec de la connexion : ' . mysqli_connect_error());
}

echo 'Connexion réussie !';

mysqli_close($link);
?>