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
        form.style.display = (form.style.display === "none" || form.style.display === "") ? "flex" : "none";
    } 
    
    else 
    {
        console.error("Element #ism-appointmentForm not found");
    }
}

function ism_addAppointment() 
{
    let title = document.getElementById("ism-appointmentTitle").value;
    let dateTime = document.getElementById("ism-appointmentDateTime").value;

    if (title.trim() === "" || dateTime.trim() === "") 
    {
        alert("Please complete all fields.");
        return;
    }

    let appointments = JSON.parse(localStorage.getItem("ism-appointments")) || [];
    let appointment = { title, dateTime };
    appointments.push(appointment);
    localStorage.setItem("ism-appointments", JSON.stringify(appointments));

    document.getElementById("ism-appointmentTitle").value = "";
    document.getElementById("ism-appointmentDateTime").value = "";
    ism_toggleForm();
    ism_loadAppointments();
}

function ism_formatDateTime(dateTime) 
{
    let date = new Date(dateTime);
    let options = { year: 'numeric', month: 'long', day: 'numeric', hour: '2-digit', minute: '2-digit' };
    return date.toLocaleDateString('en-US', options);
}

function ism_loadAppointments() 
{
    let appointmentsList = document.getElementById("ism-appointmentsList");
    appointmentsList.innerHTML = "";

    let appointments = JSON.parse(localStorage.getItem("ism-appointments")) || [];

    appointments.forEach((appointment, index) => 
    {
        let appointmentDiv = document.createElement("div");
        appointmentDiv.classList.add("ism-appointment");
        appointmentDiv.innerHTML = `
            <span><strong>${appointment.title}</strong></span><br>
            <span>${ism_formatDateTime(appointment.dateTime)}</span>
            <div>
                <button class="ism-edit-button" onclick="ism_editAppointment(${index})">✏️</button>
                <button class="ism-delete-button" onclick="ism_deleteAppointment(${index})">❌</button>
            </div>
        `;

        appointmentsList.appendChild(appointmentDiv);
    });
}

function ism_deleteAppointment(index) 
{
    let appointments = JSON.parse(localStorage.getItem("ism-appointments")) || [];
    appointments.splice(index, 1);
    localStorage.setItem("ism-appointments", JSON.stringify(appointments));
    ism_loadAppointments();
}

document.addEventListener("DOMContentLoaded", function() 
{
    let dateTimeInput = document.getElementById("ism-appointmentDateTime");

    if (dateTimeInput) {
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

function ism_editAppointment(index) 
{
    let appointments = JSON.parse(localStorage.getItem("ism-appointments")) || [];
    let appointment = appointments[index];

    let newTitle = prompt("Modifier le titre :", appointment.title);
    if (newTitle === null || newTitle.trim() === "") return; 

    let newDateTime = prompt("Modifier la date et l'heure (AAAA-MM-JJTHH:MM) :", appointment.dateTime);
    if (newDateTime === null || newDateTime.trim() === "") return;

    appointments[index] = { title: newTitle, dateTime: newDateTime };
    localStorage.setItem("ism-appointments", JSON.stringify(appointments));
    ism_loadAppointments();
}
