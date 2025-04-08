# 📜 Requirements

To build your embedded device, you need to have a clear idea of the requirements. On this page, you can describe the requirements of your embedded device. This includes the requirements from DLO, but also your own requirements.

Add some images! 😉

| Requirement ID# | Requirement                                                                 | MoSCoW | Compliant |
|---------------|-----------------------------------------------------------------------------|--------|-----------|
| EMBRQ#01      | The embedded device acts as a client and sends measured sensor data to the application backend over http or https. | MUST   | YES        |


### EMBRQ#01

The embedded device acts as a client and sends measured sensor data to the application backend over http or https.

Code:
```c++
#include <WiFiManager.h>
#include <HTTPClient.h>

// The rest of the includes

const char* SENSOR_API_URL = "http://100.74.252.69/Database/SensorData/insert_sensor_data.php";
const int DEVICE_ID = 1;

// The rest of the constants

// Sends sensor data to the API server
 void sendSensorData(int sensor_id, float value) 
 {
   // Only send if WiFi is connected
   if (WiFi.status() != WL_CONNECTED) return;
 
   HTTPClient http;
   http.begin(SENSOR_API_URL);
   
   // Format POST data with all sensor values
   String postData = "device_id=" + String(DEVICE_ID) +
                    "&time_stamp=" + getCurrentTime() +
                    "&sensor_id_1=1&value_1=" + (sensor_id == BUTTON_SENSOR_ID ? String(value) : "0.0") +
                    "&sensor_id_2=2&value_2=" + (sensor_id == PIR_SENSOR_ID ? String(value) : "0.0") +
                    "&sensor_id_3=3&value_3=" + (sensor_id == LDR_SENSOR_ID ? String(value) : "0.0");
   
   http.POST(postData);
   http.end();
 }

// The rest of the functions

```