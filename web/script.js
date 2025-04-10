/*
 Description: This script handles the front-end interactions of the web application,
 including the loading screen, the terminal effect for the main page, and the 
 full CRUD management of appointments via a backend PHP API.
 Author: Victor HOEGY
 Date : 12/02/25
 */

document.addEventListener("DOMContentLoaded", function () {
  // Show loading screen and hide it after a delay
  const loadingScreen = document.getElementById("loading-screen");
  const mainContent = document.getElementById("main-content");

  if (loadingScreen && mainContent) {
    setTimeout(() => {
      loadingScreen.style.transition = "opacity 1s ease-out";
      loadingScreen.style.opacity = "0";

      setTimeout(() => {
        loadingScreen.style.display = "none";
        mainContent.style.display = "block";
      }, 1000);
    }, 3800);
  }

  // Terminal overlay elements
  const button = document.getElementById("omnissiahButton");
  const overlay = document.getElementById("omnissiah-overlay");
  const text = document.getElementById("terminal-text");
  const loading = document.getElementById("terminal-loading");
  const close = document.getElementById("close-overlay");

  if (button && overlay && text && loading && close) {
    // Open terminal overlay with animated text
    button.addEventListener("click", () => {
      overlay.classList.remove("hidden");
      text.textContent = '';
      loading.style.width = '0%';

      let progress = 0;
      const interval = setInterval(() => {
        progress += 10;
        loading.style.width = `${progress}%`;

        if (progress >= 100) {
          clearInterval(interval);
          const lines = [
            'Accès autorisé...',
            'Connexion au Noosphère en cours...',
            'Validation du rite de l’Omnissiah...',
            '',
            'Dans Warhammer 40K, l’Omnissiah est vénéré comme une divinité par le Culte Mechanicus.',
            'Chaque machine est sacrée, abritant un Machine Spirit.',
            '',
            'Les Tech-Priests exécutent des rites pour activer ou réparer ces machines,',
            'croyant que l’Omnissiah est l’avatar de la connaissance divine.',
            '',
            '[ Transmission terminée. ]'
          ];
          typeLines(lines, text);
        }
      }, 150);
    });

    // Close terminal overlay
    close.addEventListener("click", () => {
      overlay.classList.add("hidden");
    });

// Type out lines one character at a time in a terminal style

    function typeLines(lines, element, charDelay = 30, lineDelay = 400) {
      let i = 0;
      element.textContent = '';
      const nextLine = () => {
        if (i >= lines.length) return;
        let line = lines[i];
        let j = 0;
        const typer = setInterval(() => {
          element.textContent += line.charAt(j);
          j++;
          if (j >= line.length) {
            clearInterval(typer);
            element.textContent += '\n';
            setTimeout(nextLine, lineDelay);
          }
        }, charDelay);
        i++;
      };
      nextLine();
    }
  }
});

// Inside Spirit Machine Logic

// Toggle the visibility of the appointment form
 
function ism_toggleForm() {
  const form = document.getElementById("ism-appointmentForm");
  if (form) {
    form.style.display = (form.style.display === "none" || form.style.display === "") ? "flex" : "none";
  } else {
    console.error("Element #ism-appointmentForm not found");
  }
}

// Add a new appointment via POST request
 
function ism_addAppointment() {
  const task = document.getElementById("ism-appointmentTitle").value;
  const date_hour = document.getElementById("ism-appointmentDateTime").value;
  const device_id = 1;

  if (task.trim() === "" || date_hour.trim() === "") {
    alert("Veuillez remplir tous les champs.");
    return;
  }

  fetch("Database/Appointment/insert_appointment.php", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: `task=${encodeURIComponent(task)}&date_hour=${encodeURIComponent(date_hour)}&device_id=${device_id}`
  })
    .then(response => response.json())
    .then(data => {
      if (data.status === "success") {
        document.getElementById("ism-appointmentTitle").value = "";
        document.getElementById("ism-appointmentDateTime").value = "";
        ism_toggleForm();
        ism_loadAppointments();
      }
    })
    .catch(error => console.error("Erreur AJAX :", error));
}


// Format date/time to readable format (not currently used)
 
function ism_formatDateTime(dateTime) {
  const date = new Date(dateTime);
  const options = {
    year: 'numeric',
    month: 'long',
    day: 'numeric',
    hour: '2-digit',
    minute: '2-digit'
  };

  return date.toLocaleDateString('en-US', options);
}


// Load appointments from the database
 
function ism_loadAppointments() {
  const appointmentsList = document.getElementById("ism-appointmentsList");
  appointmentsList.innerHTML = "";

  fetch("Database/Appointment/recover_appointment.php")
    .then(response => response.json())
    .then(appointments => {
      appointmentsList.innerHTML = "";
      appointments.forEach(appointment => {
        const appointmentDiv = document.createElement("div");
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

// Delete an appointment via DELETE request
 
function ism_deleteAppointment(appointment_id) {
  fetch("Database/Appointment/delete_appointment.php", {
    method: "DELETE",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ appointment_id })
  })
    .then(response => response.json())
    .then(data => {
      if (data.status === "success") {
        ism_loadAppointments();
      }
    })
    .catch(error => console.error("Erreur AJAX : ", error));
}

// Initialize appointments on page load
document.addEventListener("DOMContentLoaded", function () {
  const appointmentsList = document.getElementById("ism-appointmentsList");

  if (appointmentsList) {
    ism_loadAppointments();

    const dateTimeInput = document.getElementById("ism-appointmentDateTime");
    if (dateTimeInput) {
      dateTimeInput.addEventListener("focus", function () {
        if (this.showPicker) {
          this.showPicker();
        }
      });
    } else {
      console.error("L'élément #ism-appointmentDateTime est introuvable !");
    }
  }
});

// Show the edit form for a specific appointment
 
function ism_editAppointment(appointment_id, currentTitle, currentDateTime) {
  document.querySelectorAll('.edit-btn, .delete-btn').forEach(btn => btn.style.display = 'none');
  const editForm = document.createElement("div");
  editForm.classList.add("ism-edit-form");

  editForm.innerHTML = `
    <label>Edit title:</label>
    <input type="text" id="ism-edit-title" value="${currentTitle}">
    <label>Change date and time:</label>
    <input type="datetime-local" id="ism-edit-datetime" value="${currentDateTime}">
    <button onclick="ism_saveEditedAppointment(${appointment_id})">💾 Save</button>
    <button onclick="this.parentElement.remove()">❌ Cancel</button>
  `;

  const appointmentDiv = document.querySelector(`[data-id='${appointment_id}']`);
  if (appointmentDiv) {
    appointmentDiv.appendChild(editForm);

    const dateTimeInput = document.getElementById("ism-edit-datetime");
    if (dateTimeInput) {
      dateTimeInput.addEventListener("click", function () {
        if (this.showPicker) {
          this.showPicker();
        }
      });
    }
  } else {
    console.error("L'élément avec l'ID donné n'a pas été trouvé");
  }
}


//Save the edited appointment via PUT request

function ism_saveEditedAppointment(appointment_id) {
  const newTitle = document.getElementById("ism-edit-title").value;
  const newDateTime = document.getElementById("ism-edit-datetime").value;

  if (newTitle.trim() === "" || newDateTime.trim() === "") {
    alert("Tous les champs doivent être remplis !");
    return;
  }

  fetch("Database/Appointment/update_appointment.php", {
    method: "PUT",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      appointment_id,
      task: newTitle,
      date_hour: newDateTime
    })
  })
    .then(response => response.json())
    .then(data => {
      if (data.status === "success") {
        ism_loadAppointments();
      }
    })
    .catch(error => {
      console.error("Erreur AJAX : ", error);
      alert("Une erreur est survenue, veuillez réessayer.");
    });
}
