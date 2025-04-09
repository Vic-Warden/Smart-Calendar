# API reference

Write here your own content!

Cette API permet de gérer les rendez-vous enregistrés dans la base de données. Elle est utilisée par mon site pour ajouter, récupérer, modifier ou supprimer des données sans recharger la page. L’API est développée en PHP, et toutes les réponses sont retournées au format JSON. Les requêtes sont envoyées avec fetch() côté JavaScript.

[Voir les fichiers liés aux appointments sur GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/tree/main/web/Database/Appointment?ref_type=heads)


### Insert a New Appointment

**Endpoint**: /insert_appointment.php  
**Method**: POST  
**Description**: Crée un nouvel appointment (rendez-vous) pour un device

### Request Parameters (body: `application/x-www-form-urlencoded`)

| Parameter   | Type     | Required | Description                                                        |
|-------------|----------|----------|--------------------------------------------------------------------|
| `task`      | `string` | Yes      | Description de la tâche à accomplir                                |
| `date_hour` | `string` | Yes      | Date et heure du rendez-vous (format : `YYYY-MM-DD HH:MM:SS`)     |
| `device_id` | `int`    | Yes      | ID de l'appareil (doit exister dans la table `Device`)             |


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

[insert_appointment.php sur GitLab](https://gitlab.fdmci.hva.nl/IoT/2024-2025-semester-2/individual-project/buudiizaaduu29/-/blob/main/web/Database/Appointment/insert_appointment.php?ref_type=heads)