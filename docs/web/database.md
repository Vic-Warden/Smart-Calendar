# Database

Write here your own content!

![Menu](ERD Diagram.drawio.png)

For the WeMos D1 Mini table, it represents the microcontroller that executes the programmed appointments and integrates several sensors to collect environmental data.

I've chosen device_id as the primary key to uniquely identify each WeMos module in the system, thus avoiding any conflict if several devices exist at the same time. The name and ip_address attributes are there to differentiate the devices and enable network communication if required.

The relationship between WeMos D1 Mini and Sensor is important, as the WeMos is the central unit that manages the connected sensors.

A WeMos can manage several sensors (0,N), while each sensor is linked to a single WeMos (1,1). This is why device_id is a foreign key in the Sensor table.

With this structure, I ensure that each sensor is associated with a specific WeMos, thus avoiding any ambiguity when retrieving data. This relationship is called Integral, since it reflects the fact that the WeMos D1 Mini manages the sensors connected to it.

For the Sensor table, I have chosen to group all sensor types in a single table, rather than creating a separate table for each type.

The sensor_id attribute is the primary key that uniquely identifies each sensor, while the type attribute is used to differentiate between sensors (e.g. presence sensor, light sensor, button).

I've also added the activation_threshold attribute, which defines a specific value at which an action can be triggered.

This avoids redundancy and makes it easier to add new sensor types without having to modify the database structure.

The relationship between Sensor and Sensor_Data ensures that each sensor can generate multiple data records over time (0,N), but that each piece of data belongs to a single sensor (1,1).

This is why sensor_id is a foreign key in the Sensor_Data table. The data_id attribute is the primary key of Sensor_Data, uniquely identifying each collected measurement.

The value, timestamp and button_state attributes store the data actually collected by the sensor.

The Generate relationship describes how a sensor continuously generates data, ensuring that the system can track and record environmental changes in real time.

By structuring the database in this way, I ensure that all components are well connected, avoiding redundancy and maintaining flexibility.

The relationships between tables are designed to reflect the reality of the system, ensuring that each appointment is linked to a specific WeMos, that each WeMos integrates several sensors and that each sensor generates several data records.

This structure enables the system to automate task execution efficiently and collect the necessary environmental data without adding unnecessary complexity.