#ifndef PROJEKTI_H
#define PROJEKTI_H

typedef struct
{
    int month, day, hour;
} Time;

typedef struct
{
    char description[30];
    Time time;
} Appointment;

// add and cancel appointments
Appointment *add(char *description, int month, int day, int hour, Appointment *calendar);
Appointment *cancel(int month, int day, int hour, Appointment *calendar);

// save and load calendar
int save_calendar(const char *filename, Appointment *calendar);
Appointment *load_calendar(const char *filename, Appointment *calendar);

void print_calendar(Appointment *calendar);

void quit_program(Appointment *calendar);

// helper functions
int cal_size(Appointment *calendar);
Appointment *find_appointment(int month, int day, int hour, Appointment *calendar);
int compare_time(const void *a, const void *b);
int check_params(int month, int day, int hour);

#endif