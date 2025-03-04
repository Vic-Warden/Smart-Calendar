document.addEventListener("DOMContentLoaded", function() 
{
    setTimeout(() => 
    {
        let loadingScreen = document.getElementById("loading-screen");
        loadingScreen.style.transition = "opacity 1s ease-out";
        loadingScreen.style.opacity = "0";

        setTimeout(() => 
        {
            loadingScreen.style.display = "none";
            document.getElementById("main-content").style.display = "block";
        }, 1000);
    }, 3800);
});

/*Inside Spirit Machine*/

function ism_toggleForm() 
{
    let form = document.getElementById("ism-appointmentForm");
    
    if (form) 
    {
        form.style.display = 
        (form.style.display === "none" || form.style.display === "") 
        ? "flex" : "none";
    } 

    else 
    {
        console.error("Element #ism-appointmentForm not found");
    }
}

function ism_addAppointment() 
{
    let task = document.getElementById("ism-appointmentTitle").value;
    let date_hour = document.getElementById("ism-appointmentDateTime").value;
    let device_id = 1;

    if (task.trim() === "" || date_hour.trim() === "") 
    {
        alert("Veuillez remplir tous les champs.");
        return;
    }

    fetch("Database/Appointment/insert_appointment.php",
    {
        method: "POST",
        headers: 
        { 
            "Content-Type": "application/x-www-form-urlencoded" 
        },
            body: `task=${encodeURIComponent(task)}&date_hour=${encodeURIComponent(date_hour)}&device_id=${device_id}`
    })

    .then(response => response.json())
    .then(data => 
    {
        if (data.status === "success") 
        {
            document.getElementById("ism-appointmentTitle").value = "";
            document.getElementById("ism-appointmentDateTime").value = "";
            ism_toggleForm();
            ism_loadAppointments();
        }
    })
    .catch(error => console.error("Erreur AJAX :", error));
}


function ism_formatDateTime(dateTime) 
{
    let date = new Date(dateTime);
    let options = 
    { 
        year: 'numeric', 
        month: 'long', 
        day: 'numeric', 
        hour: '2-digit', 
        minute: '2-digit' 
    };
    
    return date.toLocaleDateString('en-US', options);
}

function ism_loadAppointments() 
{
    let appointmentsList = document.getElementById("ism-appointmentsList");
    appointmentsList.innerHTML = "";

    fetch("Database/Appointment/recover_appointment.php")
    .then(response => response.json())
    .then(appointments => {
        appointmentsList.innerHTML = "";

        appointments.forEach(appointment => 
        {
            let appointmentDiv = document.createElement("div");
            appointmentDiv.classList.add("ism-appointment");
            appointmentDiv.setAttribute("data-id", appointment.appointment_id);

            appointmentDiv.innerHTML = `
                <span><strong>${appointment.task}</strong></span><br>
                <span>${appointment.date_hour}</span>
                <div>
                    <button class="ism-edit-button" onclick="ism_editAppointment(${appointment.appointment_id}, '${appointment.task}', '${appointment.date_hour}')">✏️</button>
                    <button class="ism-delete-button" onclick="ism_deleteAppointment(${appointment.appointment_id})">❌</button>
                </div>
            `;

            appointmentsList.appendChild(appointmentDiv);
        });
    })
    .catch(error => console.error("Erreur lors du chargement des rendez-vous :", error));
}


function ism_deleteAppointment(appointment_id) 
{
    fetch("Database/Appointment/delete_appointment.php", 
    {
        method: "POST",
        headers: 
        {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        
        body: `appointment_id=${encodeURIComponent(appointment_id)}`
    })
    .then(response => response.json())
    .then(data => 
    {
        console.log("Réponse du serveur : ", data);

        if (data.status === "success") 
        {
            ism_loadAppointments();
        }
    })
    .catch(error => console.error("Erreur AJAX : ", error));
}

document.addEventListener("DOMContentLoaded", function() 
{
    ism_loadAppointments();
    let dateTimeInput = document.getElementById("ism-appointmentDateTime");

    if (dateTimeInput) 
    {
        dateTimeInput.addEventListener("focus", function() 
        {
            if (this.showPicker) 
            {
                this.showPicker(); 
            }
        });
    }

    else 
    {
        console.error("L'élément #ism-appointmentDateTime est introuvable !");
    }
});

function ism_editAppointment(appointment_id, currentTitle, currentDateTime) 
{

    let editForm = document.createElement("div");
    editForm.classList.add("ism-edit-form");

    editForm.innerHTML = `
        <label>Edit title:</label>
        <input type="text" id="ism-edit-title" value="${currentTitle}">

        <label>Change date and time:</label>
        <input type="datetime-local" id="ism-edit-datetime" value="${currentDateTime}">

        <button onclick="ism_saveEditedAppointment(${appointment_id})">💾 Save</button>
        <button onclick="this.parentElement.remove()">❌ Cancel</button>
    `;

    let appointmentDiv = document.querySelector(`[data-id='${appointment_id}']`);
    
    if (appointmentDiv) 
    {
        appointmentDiv.appendChild(editForm);

        let dateTimeInput = document.getElementById("ism-edit-datetime");

        dateTimeInput.addEventListener("click", function() 
        {
            if (this.showPicker) 
            {
                this.showPicker();
            }
        });

    } 
    
    else 
    {
        console.error("L'élément avec l'ID donné n'a pas été trouvé");
    }
}

function ism_saveEditedAppointment(appointment_id) 
{
    let newTitle = document.getElementById("ism-edit-title").value;
    let newDateTime = document.getElementById("ism-edit-datetime").value;

    if (newTitle.trim() === "" || newDateTime.trim() === "") 
    {
        alert("Tous les champs doivent être remplis !");
        return;
    }

    fetch("Database/Appointment/update_appointment.php", 
    {
        method: "POST",
        
        headers: 
        {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: `appointment_id=${encodeURIComponent(appointment_id)}&task=${encodeURIComponent(newTitle)}&date_hour=${encodeURIComponent(newDateTime)}`
    })
    .then(response => response.json())
    .then(data => 
    {
        console.log("Réponse du serveur : ", data);
        if (data.status === "success") 
        {
            ism_loadAppointments();
        }
    })
    .catch(error => {
        console.error("Erreur AJAX : ", error);
        alert("Une erreur est survenue, veuillez réessayer.");
    });
}
