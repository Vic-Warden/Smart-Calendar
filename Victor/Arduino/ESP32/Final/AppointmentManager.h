#ifndef APPOINTMENT_MANAGER_H
#define APPOINTMENT_MANAGER_H

#include "Config.h"

extern DynamicJsonDocument appointments;

void refreshAppointments(unsigned long currentMillis);
void fetchAppointments();
void checkForAppointmentChanges(DynamicJsonDocument& newAppointments);

#endif
