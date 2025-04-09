# Web Code

Write here your own content!

### Project Structure

The project is structured in a clean and modular way :

```
📁 root/
├── index.html                        # Main homepage with loading screen + terminal
├── inside_spirit_machine.html        # Calendar + sensor-based interface
├── style.css                         # Custom design + Tailwind overrides
├── script.js                         # Front-end logic & API calls
├── /images/tech_priest.png           # Static assets
├── /models/Astra_Militarum.stl       # 3D STL model for viewer
└── Database/
    └── Appointment/
        ├── insert_appointment.php    # POST: Add a new appointment
        ├── recover_appointment.php   # GET: Retrieve all appointments
        ├── update_appointment.php    # PUT: Edit an appointment
        └── delete_appointment.php    # DELETE: Remove an appointment
```

---

I followed code practices for every language used :

### JavaScript
- All functions are written in camelCase
- API calls use fetch() with clear HTTP methods
- DOM interactions are grouped logically
- Code is fully commented and structured

### PHP
- Each route is in its own file (RESTful style)
- Responses are returned in consistent JSON format
- Prepared statements are used to prevent SQL injection

### HTML / CSS
- Semantic HTML structure (no inline styles)
- CSS is modular, separated from HTML
- Used Tailwind CSS + custom utility classes
- Animations and transitions are abstracted with clean class names

All source files include a comment header with:  
- Description  
- Author   

And the key functions are explained with inline comments

---

### JavaScript Adding an Appointment

This function collects user input and sends a POST request to the backend API.

```js
function ism_addAppointment() {
  const task = document.getElementById("ism-appointmentTitle").value;
  const date_hour = document.getElementById("ism-appointmentDateTime").value;
  const device_id = 1;

  fetch("Database/Appointment/insert_appointment.php", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: `task=${task}&date_hour=${date_hour}&device_id=${device_id}`
  })
  .then(response => response.json())
  .then(data => {
    if (data.status === "success") {
      ism_loadAppointments();
    }
  });
}
```

---

### PHP

This endpoint receives appointment data and inserts it into the database.

```php
<?php
// Include the database connection file
require __DIR__ . '/../Connection/database_connection.php';

// Check if the request method is POST
if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    // Check if task is set and not empty
    if (isset($_POST['task']) && trim($_POST['task']) !== '') 
    {
        $task = trim($_POST['task']);
    } 
    
    else 
    {
        // Return error if task is missing
        echo json_encode(["status" => "error", "message" => "Task field is required"]);
        exit;
    }

    // Check if date_hour is set and not empty
    if (isset($_POST['date_hour']) && trim($_POST['date_hour']) !== '') 
    {
        $date_hour = trim($_POST['date_hour']);
    } 

    else 
    {
        // Return error if date_hour is missing
        echo json_encode(["status" => "error", "message" => "Date and time field is required"]);
        exit;
    }

    // Check if device_id is set and is a number
    if (isset($_POST['device_id']) && is_numeric($_POST['device_id'])) 
    {
        $device_id = intval($_POST['device_id']);
    } 

    else 
    {
        // Return error if device_id is missing or invalid
        echo json_encode(["status" => "error", "message" => "Device ID field is required and must be a number"]);
        exit;
    }

    // Double check if any field is still empty
    if (empty($task) || empty($date_hour) || empty($device_id)) 
    {
        echo json_encode(["status" => "error", "message" => "All fields are required"]);
        exit;
    }

    // Check if the device exists in the database
    $check_device = $link->prepare("SELECT device_id FROM Device WHERE device_id = ?");
    $check_device->bind_param("i", $device_id);
    $check_device->execute();
    $result = $check_device->get_result();

    if ($result->num_rows === 0) 
    {
        // Return error if device is not found
        echo json_encode(["status" => "error", "message" => "Wemos not connected"]);
        exit;
    }

    // Set default note value
    $note = "active";

    // Insert the new appointment into the database
    $stmt = $link->prepare("INSERT INTO Appointment (task, date_hour, note, device_id) VALUES (?, ?, ?, ?)");
    $stmt->bind_param("sssi", $task, $date_hour, $note, $device_id);

    if ($stmt->execute()) 
    {
        // Return success with the new appointment ID
        echo json_encode(["status" => "success", "message" => "Appointment added", "appointment_id" => $stmt->insert_id]);
    } 

    else 
    {
        // Return error if insertion failed
        echo json_encode(["status" => "error", "message" => "Error: " . $stmt->error]);
    }

    // Close statements and connection
    $stmt->close();
    $check_device->close();
    $link->close();
} 

else 
{
    // Return error if request method is not POST
    echo json_encode(["status" => "error", "message" => "Invalid request"]);
}
?>
```

---

### CSS

This example shows how I style a full-width background and a custom button

```css
.background-section {
  width: 100%;
  height: 100vh;
  background-image: url('Home_page.png');
  background-size: cover;
  background-position: center;
  display: flex;
  justify-content: center;
  align-items: flex-start;
}

.button {
  background-color: #8b0000;
  color: white;
  padding: 10px 20px;
  border-radius: 5px;
  text-transform: uppercase;
}
```

[GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/tree/main/web?ref_type=heads) 