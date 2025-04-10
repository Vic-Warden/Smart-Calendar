# API reference

Write here your own content!

This API is used to manage appointments stored in the database. It is used by my site to add, retrieve, modify or delete data without reloading the page. The API is developed in PHP, and all responses are returned in JSON format. Requests are sent using fetch() on the JavaScript side.

---

## Database Connection

**Endpoint :** http://localhost/Database/Connection/database_connection.php  

**Description :** Establishes a connection to the MariaDB database using mysqli

This file is included in all PHP files that interact with the database.  
If the connection fails, the script stops and returns an error message.

## Parameters

| Variable     | Value        | Description                   |
|--------------|--------------|-------------------------------|
| $host      | mariadb    | Hostname of the database      |
| $user      | root       | Database username             |
| $password  | 7YKyE8R2AhKzswfN | Database password        |
| $database  | iot        | Name of the database          |

[database_connection.php  on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Connection/database_connection.php?ref_type=heads)

---


## Insert a New Appointment

**Endpoint :** http://localhost/Database/Appointment/insert_appointment.php  
**Method :** POST  
**Description :** Create a new appointment for a device

## Request Parameters body: application/x-www-form-urlencoded

| Parameter   | Type     | Required | Description                                                        |
|-------------|----------|----------|--------------------------------------------------------------------|
| task      | string | Yes      | Task                                |
| date_hour | string | Yes      | Date and hour format: YYYY-MM-DD HH:MM:SS     |
| device_id | int    | Yes      | Device ID             |


## Success Response

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

## Get Last Appointment

**Endpoint :** http://localhost/Database/Appointment/get_last_appointment.php  
**Method :** GET  
**Description :** Retrieve the last appointment saved in the system

## Request Parameters

No parameters required

## Success Response

```json
{
  "appointment_id": 3,
  "task": "Ready to die for the Emperor",
  "date_hour": "2025-03-20 17:05:00",
  "device_id": 1
}
```

## Error Response

```json
{
  "status": "error",
  "message": "No appointment found"
}
```
[get_last_appointment.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Appointment/get_last_appointment.php?ref_type=heads)

## Update Appointment

**Endpoint :** http://localhost/Database/Appointment/update_appointment.php  
**Method :** PUT  
**Description :** Update an existing appointment by ID

## Request Body Content-Type: application/json

| Parameter        | Type     | Required | Description                                       |
|------------------|----------|----------|---------------------------------------------------|
| appointment_id | int    | Yes      | ID of the appointment to update                   |
| task           | string | Yes      | Updated task description                          |
| date_hour      | string | Yes      | Updated date and time format: YYYY-MM-DD HH:MM:SS |

## Success Response

```json
{
  "status": "success",
  "message": "Appointment updated"
}
```

## Error Responses

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

## Get Last Updated Appointment

**Endpoint :** http://localhost/Database/Appointment/get_last_updated_appointment.php  
**Method :** GET  
**Description :** Retrieve the last updated appointment saved in a JSON file.

## Request Parameters

No parameters required

## Success Response

```json
{
  "appointment_id": 1,
  "task": "Fear ensures loyalty ! Where do you need me ?kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk",
  "date_hour": "2025-03-14 15:00:00",
  "device_id": 1
}
```

## Error Response

```json
{
  "status": "error",
  "message": "No updated appointment found"
}
```

[get_last_updated_appointment.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Appointment/get_last_updated_appointment.php?ref_type=heads)


## Delete Appointment

**Endpoint :** http://localhost/Database/Appointment/delete_appointment.php  
**Method :** DELETE  
**Description :** Delete an appointment by ID and return its data

## Request Body Content-Type: application/json

| Parameter        | Type     | Required | Description                        |
|------------------|----------|----------|------------------------------------|
| appointment_id | int    | Yes      | ID of the appointment to delete    |

## Success Response

```json
{
  "status": "success",
  "message": "Appointment deleted",
  "deleted_appointment": {
    "appointment_id": 1,
    "task": "Fear ensures loyalty ! Where do you need me ?kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk",
    "date_hour": "2025-03-14 15:00:00",
    "device_id": 1,
    "note": "deleted"
  }
}
```

## Error Responses

```json
{
  "status": "error",
  "message": "Invalid request method, use DELETE"
}
```

```json
{
  "status": "error",
  "message": "Valid appointment ID is required."
}
```

```json
{
  "status": "error",
  "message": "Appointment not found"
}
```

```json
{
  "status": "error",
  "message": "Error: <SQL error message>"
}
```
[delete_appointment.php  on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Appointment/delete_appointment.php?ref_type=heads)


## Get Last Deleted Appointment

**Endpoint :** http://localhost/Database/Appointment/get_last_deleted_appointment.php  
**Method :** GET  
**Description :** Retrieve the last deleted appointment saved in a JSON file.

## Request Parameters

No parameters required

## Success Response

```json
{
  "status": "success",
  "message": "Appointment deleted",
  "deleted_appointment": {
    "appointment_id": 1,
    "task": "Fear ensures loyalty ! Where do you need me ?kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk",
    "date_hour": "2025-03-14 15:00:00",
    "device_id": 1,
    "note": "deleted"
  }
}
```

## Error Response

```json
{
  "status": "error",
  "message": "No deleted appointment found"
}
```

[get_last_deleted_appointment.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Appointment/get_last_deleted_appointment.php?ref_type=heads)

## Get All Appointments

**Endpoint**: http://localhost/Database/Appointment/recover_appointment.php  
**Method**: GET  
**Description**: Retrieve all appointments from the database.

## Request Parameters

No parameters required

## Success Response

```json
[
  {
    "appointment_id": 5,
    "task": "Die for him",
    "date_hour": "2025-04-10 10:00:00",
    "note": "active",
    "device_id": 1
  },
  {
    "appointment_id": 7,
    "task": "By the Emperor it will be done !",
    "date_hour": "2025-04-11 15:30:00",
    "note": "active",
    "device_id": 1
  }
]
```

[recover_appointment.php  on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Appointment/recover_appointment.php?ref_type=heads)

---

## Insert Device

**Endpoint :** http://localhost/Database/Device/insert_device.php  
**Method :** POST  
**Description :** Adds a new device or updates its IP address if it already exists.

## Request Parameters body: application/x-www-form-urlencoded

| Parameter     | Type     | Required | Description               |
|---------------|----------|----------|---------------------------|
| name        | string | Yes      | Name of the device        |
| ip_address  | string | Yes      | IP address of the device  |

## Success Responses

```json
{
  "status": "success",
  "message": "New device added successfully.",
  "device": {
    "device_id": 1,
    "name": "ESP32",
    "ip_address": "100.74.254.22"
  }
}
```

## Error Responses

```json
{
  "status": "error",
  "message": "Please fill in all required fields."
}

```
```json
{
  "status": "error",
  "message": "SQL error message"
}

```

[insert_device.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Device/insert_device.php?ref_type=heads)

## Delete Device

**Endpoint :** http://localhost/Database/Device/delete_device.php  
**Method :** POST  
**Description :** Delete a device from the database using its ID.

## Request Parameters body: application/x-www-form-urlencoded

| Parameter     | Type     | Required | Description                    |
|---------------|----------|----------|--------------------------------|
| device_id   | int    | Yes      | ID of the device to be deleted |

## Success Response

```json
{
  "status": "success",
  "message": "Device with ID 3 successfully deleted."
}
```

## Error Responses

```json
{
  "status": "error",
  "message": "Missing or invalid device ID."
}
```

```json
{
  "status": "error",
  "message": "Device with ID 3 does not exist."
}
```

```json
{
  "status": "error",
  "message": "Error while deleting the device."
}
```

```json
{
  "status": "error",
  "message": "Invalid request method."
}
```

[delete_device.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Device/delete_device.php?ref_type=heads)

## Get All Devices

**Endpoint :** http://localhost/Database/Device/recover_device.php  
**Method :** GET  
**Description :** Retrieve all devices from the database and save them in a JSON file.

## Request Parameters

No parameters required

## Success Response

```json
{
  "status": "success",
  "devices": [
    {
      "device_id": 1,
      "name": "ESP32",
      "ip_address": "100.74.254.22"
    }
  ]
}
```

## Error Response

```json
{
  "status": "error",
  "message": "No devices found."
}
```

[recover_device.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Device/recover_device.php?ref_type=heads)

---

## Insert Sensor

**Endpoint :** http://localhost/Database/Sensor/insert_sensor.php  
**Method :** POST  
**Description :** Add a new sensor for a specific device.

## Request Parameters body: application/x-www-form-urlencoded

| Parameter              | Type     | Required | Description                                |
|------------------------|----------|----------|--------------------------------------------|
| name                 | string | Yes      | Name of the sensor                         |
| type                 | string | Yes      | Type of the sensor (e.g., temperature)     |
| activation_threshold | float  | No       | Value at which the sensor activates        |
| device_id            | int    | Yes      | ID of the device the sensor is linked to   |

## Success Response

```json
{
  "status": "success",
  "message": "Sensor added successfully.",
  "sensor": {
    "sensor_id": 3,
    "name": "Photoresistor",
    "type": "Light",
    "activation_threshold": 700,
    "device_id": 1
  }
}
```

## Error Responses

```json
{
  "status": "error",
  "message": "A sensor with this name already exists for this device."
}
```

```json
{
  "status": "error",
  "message": "Please fill in all required fields."
}
```

```json
{
  "status": "error",
  "message": "SQL error message"
}
```

[insert_sensor.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Sensor/insert_sensor.php?ref_type=heads)

## Delete Sensor

**Endpoint :** http://localhost/Database/Sensor/delete_sensor.php  
**Method :** POST  
**Description :** Delete a sensor from the database using its ID.

## Request Parameters body: application/x-www-form-urlencoded

| Parameter     | Type     | Required | Description                    |
|---------------|----------|----------|--------------------------------|
| sensor_id   | int    | Yes      | ID of the sensor to be deleted |

## Success Response

```json
{
  "status": "success",
  "message": "Sensor with ID 4 successfully deleted."
}
```

## Error Responses

```json
{
  "status": "error",
  "message": "Missing or invalid sensor ID."
}
```

```json
{
  "status": "error",
  "message": "Sensor with ID 4 does not exist."
}
```

```json
{
  "status": "error",
  "message": "Error while deleting the sensor."
}
```

```json
{
  "status": "error",
  "message": "Invalid request method."
}
```

[delete_sensor.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Sensor/delete_sensor.php?ref_type=heads)

## Get All Sensors

**Endpoint :** http://localhost/Database/Sensor/recover_sensor.php  
**Method :** GET  
**Description :** Retrieve all sensors from the database.

## Request Parameters

No parameters required

## Success Response

```json
{
  "status": "success",
  "sensors": [
    {
      "sensor_id": 1,
      "name": "Button",
      "type": "Button",
      "activation_threshold": 1,
      "device_id": 1
    },
    {
      "sensor_id": 2,
      "name": "PIR",
      "type": "Presence",
      "activation_threshold": 0,
      "device_id": 1
    },
    {
      "sensor_id": 3,
      "name": "Photoresistor",
      "type": "Light",
      "activation_threshold": 700,
      "device_id": 1
    }
  ]
}
```

## Error Responses

```json
{
  "status": "error",
  "message": "No sensors found."
}
```

```json
{
  "status": "error",
  "message": "Invalid request."
}
```

[recover_sensor.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Sensor/recover_sensor.php?ref_type=heads)

---

## Insert Sensor Data 3 Sensors

**Endpoint :** http://localhost/Database/SensorData/insert_sensor_data.php  
**Method :** POST  
**Description :** Insert sensor data for 3 sensors and keep only the latest 20 records per sensor.

## Request Parameters body: application/x-www-form-urlencoded

| Parameter        | Type    | Required | Description                            |
|------------------|---------|----------|----------------------------------------|
| sensor_id_1    | int   | Yes      | First sensor ID                        |
| value_1     | float | Yes      | Value for first sensor                 |
| sensor_id_2    | int  | Yes      | Second sensor ID                       |
| value_2        | float | Yes      | Value for second sensor                |
| sensor_id_3    | int   | Yes      | Third sensor ID                        |
| value_3        |float | Yes      | Value for third sensor                 |

## Success Response

```json
{
  "status": "success",
  "message": "Sensor data inserted successfully and old entries cleaned.",
  "data": [
    {
      "sensor_id": 1,
      "value": 22.3,
      "timestamp": "2025-04-09 14:30:00"
    },
    {
      "sensor_id": 2,
      "value": 59.2,
      "timestamp": "2025-04-09 14:30:00"
    },
    {
      "sensor_id": 3,
      "value": 18.6,
      "timestamp": "2025-04-09 14:30:00"
    }
  ]
}
```

## Error Responses

```json
{
  "status": "error",
  "message": "Missing data."
}
```
```json
{
  "status": "error",
  "message": "Invalid request."
}
```

[insert_sensor_data.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/SensorData/insert_sensor_data.php?ref_type=heads)

## Get Latest Sensor Data

**Endpoint :** http://localhost/Database/SensorData/recover_sensor_data.php  
**Method :** GET  
**Description :** Retrieve the latest 20 sensor data entries, grouped by sensor

## Request Parameters

No parameters required

## Success Response

```json
{
  "status": "success",
  "sensors": {
    "1": [
      {
        "value": 0,
        "timestamp": "2025-04-01 13:39:10"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:32:22"
      },
      {
        "value": 1,
        "timestamp": "2025-04-01 13:25:18"
      },
      {
        "value": 1,
        "timestamp": "2025-04-01 13:25:12"
      },
      {
        "value": 1,
        "timestamp": "2025-04-01 13:24:59"
      },
      {
        "value": 1,
        "timestamp": "2025-04-01 13:24:53"
      }
    ],
    "2": [
      {
        "value": 1,
        "timestamp": "2025-04-01 13:39:10"
      },
      {
        "value": 1,
        "timestamp": "2025-04-01 13:32:22"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:25:18"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:25:12"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:24:59"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:24:53"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:24:51"
      }
    ],
    "3": [
      {
        "value": 0,
        "timestamp": "2025-04-01 13:39:10"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:32:22"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:25:18"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:25:12"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:24:59"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:24:53"
      },
      {
        "value": 0,
        "timestamp": "2025-04-01 13:24:51"
      }
    ]
  }
}
```

## Error Responses

```json
{
  "status": "error",
  "message": "Error while retrieving sensor data."
}
```

```json
{
  "status": "error",
  "message": "Invalid method."
}
```

[recover_sensor_data.php on GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/SensorData/recover_sensor_data.php?ref_type=heads)
