# Database

Write here your own content!

![Menu](ERD Diagram.drawio.png)

Let's start with the table, which is the most important for me, because it's the table at the heart of my project. Knowing that I'm doing the smart calendar, if I can't retrieve the appointments entered by the user, that's a bit silly...

### Table Appointments

This table will enable me to manage the various tasks that the user will enter on my website.

As you'd expect, I'll be using two keys: the first will be a main appointment_id key, which will give me a unique ID for each appointment. If the user has to see his doctor every month, even if the appointments have exactly the same names, this won't prevent me from identifying them. And for my second key, which will be my foreign key, I'll use device_id, which will link me to my Wemos D1 Mini table, enabling me to associate the corresponding task with a device. 

As for the attributes, I decided to use task for the task (obviously), date for the date (obviously again) and hour (you get the idea).

I decided to choose the relationship ( 1 : N ) because each device ( here, it doesn't really apply because I only have one wemos but if one day there's a need to add a second one, there'd be no problem ) can have several tasks.



![Menu](Database Schema.png)

As I've already explained the choices for my columns, I'll explain how I went about creating my database schema. I'm not going to dwell too much on the choice of columns, just that I've taken what I'd done before and added them. I'm mainly going to justify the choices I made for my indexes. For the date, I decided to choose only the date and time, as I thought they were the two most important things for finding an appointment.

For the sensors, the type seemed the most relevant because if I can find out what type of sensor it is, I'll be able to find out directly what data I'm receiving. 

And finally, for the sensor data, naturally the timestamp. As soon as I receive the information I can take this or that action