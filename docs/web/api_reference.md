# API reference

Write here your own content!

This API is used to manage appointments stored in the database. It is used by my site to add, retrieve, modify or delete data without reloading the page. The API is developed in PHP, and all responses are returned in JSON format. Requests are sent using fetch() on the JavaScript side.

[All appointment files](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/tree/main/web/Database/Appointment?ref_type=heads)


### Insert a New Appointment

**Endpoint**: http://localhost/Database/Appointment/insert_appointment.php  
**Method**: POST  
**Description**: Create a new appointment for a device

### Request Parameters body: application/x-www-form-urlencoded

| Parameter   | Type     | Required | Description                                                        |
|-------------|----------|----------|--------------------------------------------------------------------|
| `task`      | `string` | Yes      | Task                                |
| `date_hour` | `string` | Yes      | Date and hour format: YYYY-MM-DD HH:MM:SS     |
| `device_id` | `int`    | Yes      | Device ID             |


### Success Response

```json
{
  "status": "success",
  "message": "Appointment added",
  "appointment_id": 5
}
```
### Error Responses

```json
{
  "status": "error",
  "message": "Task field is required"
}
```
```json
{
  "status": "error",
  "message": "Date and time field is required"
}
```
```json
{
  "status": "error",
  "message": "Device ID field is required and must be a number"
}
```
```json
{
  "status": "error",
  "message": "Wemos not connected"
}
```

[insert_appointment.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Appointment/insert_appointment.php?ref_type=heads)

### Get Last Appointment

**Endpoint**: http://localhost/Database/Appointment/get_last_appointment.php  
**Method**: GET  
**Description**: Retrieve the last appointment saved in the system

### Request Parameters

No parameters required

### Success Response

```json
{
  "appointment_id": 3,
  "task": "Ready to die for the Emperor",
  "date_hour": "2025-03-20 17:05:00",
  "device_id": 1
}
```

### Error Response

```json
{
  "status": "error",
  "message": "No appointment found"
}
```
[get_last_appointment.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Appointment/get_last_appointment.php?ref_type=heads)

### Update Appointment

**Endpoint**: http://localhost/Database/Appointment/update_appointment.php  
**Method**: PUT  
**Description**: Update an existing appointment by ID

### Request Body (Content-Type: application/json)

| Parameter        | Type     | Required | Description                                       |
|------------------|----------|----------|---------------------------------------------------|
| `appointment_id` | `int`    | Yes      | ID of the appointment to update                   |
| `task`           | `string` | Yes      | Updated task description                          |
| `date_hour`      | `string` | Yes      | Updated date and time format: YYYY-MM-DD HH:MM:SS |

### Success Response

```json
{
  "status": "success",
  "message": "Appointment updated"
}
```

### Error Responses

```json
{
  "status": "error",
  "message": "Invalid request method, use PUT"
}
```

```json
{
  "status": "error",
  "message": "All fields are required."
}
```

```json
{
  "status": "error",
  "message": "SQL error: <detailed error message>"
}
```

[update_appointment.php  on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Appointment/get_last_appointment.php?ref_type=heads)