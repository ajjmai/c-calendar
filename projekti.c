#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "projekti.h"

// adds a new appointment to the calendar
Appointment *add(char *description, int month, int day, int hour, Appointment *calendar)
{
    // checks that the chosen time is not already reserved
    if (find_appointment(month, day, hour, calendar))
    {
        printf(">> Valittu aika oli jo varattu\n");
        return calendar;
    }

    int i = cal_size(calendar);
    calendar = realloc(calendar, sizeof(Appointment) * (i + 2));

    if (!calendar)
    {
        printf(">> Uuden tapahtuman lisääminen epäonnistui\n");
        return calendar;
    }
    memset(&calendar[i], 0, sizeof(Appointment));
    strcpy(calendar[i].description, description);
    calendar[i].time.month = month;
    calendar[i].time.day = day;
    calendar[i].time.hour = hour;

    // adds a new end of array marker
    calendar[i + 1].description[0] = 0;

    printf(">> Lisätty: %s %d.%d. klo %d\n", description, day, month, hour);

    return calendar;
}

// cancels a chosen appointment
Appointment *cancel(int month, int day, int hour, Appointment *calendar)
{
    Appointment *to_be_cancelled = find_appointment(month, day, hour, calendar);

    if (!to_be_cancelled)
    {
        printf(">> Valittuna aikana ei ollut tapahtumaa\n");
        return calendar;
    }

    int i = 0;
    int last = cal_size(calendar) - 1;
    while (to_be_cancelled != &calendar[i])
    {
        i++;
    }

    memmove(&calendar[i], &calendar[last], sizeof(Appointment));
    memset(&calendar[last], 0, sizeof(Appointment));
    calendar[last].description[0] = 0;

    printf(">> Peruttu tapahtuma %d.%d. klo %d\n", day, month, hour);

    return calendar;
}

// saves calendar in binary format
int save_calendar(const char *filename, Appointment *calendar)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        fprintf(stderr, ">> Tiedoston avaaminen epäonnistui\n");
        return 1;
    }

    int i = 0;
    while (calendar[i].description[0] != '\0')
    {
        fwrite(&calendar[i], 1, sizeof(Appointment), f);
        i++;
    }

    fclose(f);
    printf(">> Kalenterin tallennus onnistui\n");
    return 0;
}

// loads calendar from file
Appointment *load_calendar(const char *filename, Appointment *calendar)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        fprintf(stderr, ">> Tiedoston lataaminen epäonnistui\n");
        return calendar;
    }

    calendar = realloc(calendar, sizeof(Appointment) * 2);

    int i = 0;

    while (fread(&calendar[i], sizeof(Appointment), 1, f) == 1)
    {
        calendar = realloc(calendar, sizeof(Appointment) * (i + 2));
        i++;
    }

    calendar[i].description[0] = 0;
    fclose(f);
    printf(">> Kalenterin lataus onnistui\n");
    return calendar;
}

// prints appointments in the calendar
void print_calendar(Appointment *calendar)
{
    int i = 0;
    int size = cal_size(calendar);
    qsort(calendar, size, sizeof(Appointment), compare_time);

    printf("\n- - - - - - - - - - KALENTERI - - - - - - - - - -\n\n");
    while (i < size)
    {
        printf("%20s %02d.%02d. klo %02d\n", calendar[i].description, calendar[i].time.day, calendar[i].time.month, calendar[i].time.hour);
        i++;
    }
    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - -\n");
}

// quits progrman and frees all memory
void quit_program(Appointment *calendar)
{
    free(calendar);
}

// helper function for determining the size of the calendar
int cal_size(Appointment *calendar)
{
    int size = 0;
    while (calendar[size].description[0] != '\0')
    {
        size++;
    }
    return size;
}

// checks whether the time slot is already reserved
Appointment *find_appointment(int month, int day, int hour, Appointment *calendar)
{
    qsort(calendar, cal_size(calendar), sizeof(Appointment), compare_time);

    Appointment *key = malloc(sizeof(Appointment));
    key->time.day = day;
    key->time.month = month;
    key->time.hour = hour;

    Appointment *app = bsearch(key, calendar, cal_size(calendar), sizeof(Appointment), compare_time);
    free(key);

    return app;
}

// compares two dates
int compare_time(const void *a, const void *b)
{
    const Appointment *app1 = a;
    const Appointment *app2 = b;
    const Time t1 = app1->time;
    const Time t2 = app2->time;

    // checks months
    if (t1.month < t2.month)
        return -1;
    else if (t1.month > t2.month)
        return 1;

    // checks days
    if (t1.day < t2.day)
        return -1;
    else if (t1.day > t2.day)
        return 1;

    // checks hours
    if (t1.hour < t2.hour)
        return -1;
    else if (t1.hour > t2.hour)
        return 1;

    // times are the same
    return 0;
}

// checks that date parameters are within reasonable limits
int check_params(int month, int day, int hour)
{
    // month should be between 1 and 12
    if (month < 1 || month > 12)
        return 1;

    // day should be between 1 and 31
    if (day < 1 || day > 31)
        return 1;

    // hour should be between 0 and 23
    if (hour < 0 || hour > 23)
        return 1;

    return 0;
}

// for testing
int test(Appointment *calendar)
{
    printf("A Kampaaja 3 26 14\n");
    calendar = add("Kampaaja", 3, 26, 14, calendar);

    printf("A Luento2 1 26 12\n");
    calendar = add("Luento2", 1, 26, 12, calendar);

    printf("A Luento1 1 26 9\n");
    calendar = add("Luento1", 1, 26, 9, calendar);

    printf("A Luento3 5 10 10\n");
    calendar = add("Luento3", 5, 10, 10, calendar);

    printf("A Luento4 5 11 10\n");
    calendar = add("Luento4", 5, 11, 10, calendar);

    printf("A Kampaaja 5 10 10\n");
    calendar = add("Kampaaja", 5, 10, 10, calendar);
    printf("D 5 10 10\n");
    calendar = cancel(5, 10, 10, calendar);

    printf("A Kokous 1 26 12\n");
    calendar = add("Kokous", 1, 26, 12, calendar);

    printf("L\n");
    print_calendar(calendar);

    printf("D 1 26 12\n");
    calendar = cancel(1, 26, 12, calendar);

    printf("A Kokous 3 26 14\n");
    calendar = add("Kokous", 3, 26, 14, calendar);
    printf("D 3 26 14\n");
    calendar = cancel(3, 26, 14, calendar);

    printf("L\n");
    print_calendar(calendar);

    printf("D 1 26 12\n");
    calendar = cancel(1, 26, 12, calendar);

    printf("L\n");
    print_calendar(calendar);

    printf("W kalenteri\n");
    save_calendar("kalenteri", calendar);

    printf("A Luento2 1 26 12\n");
    calendar = add("Luento2", 1, 26, 12, calendar);

    printf("O kalenteri\n");
    calendar = load_calendar("kalenteri", calendar);

    printf("A Luento5 9 1 8\n");
    calendar = add("Luento5", 9, 1, 8, calendar);

    printf("L\n");
    print_calendar(calendar);

    printf("Q\n");
    printf(">> Poistutaan ohjelmasta\n");
    quit_program(calendar);
    return 0;
}

int main(void)
{
    // the program assumes certain maximum lenghts for text inputs
    char input[100];
    char command[1];
    int day, month, hour;
    char descpription[30];
    char file_name[20];
    Appointment *calendar = calloc(2, sizeof(Appointment));
    memset(calendar, 0, 2 * sizeof(Appointment));

    calendar[0].description[0] = 0;

    do
    {
        fgets(input, sizeof(input), stdin);

        switch (tolower((int)input[0]))
        {
        // add appointment
        case 'a':
            if (tolower((int)input[1]) != ' ')
            {
                printf(">> Virheellinen komento. Tarkista syöte\n");
                break;
            }

            if (sscanf(input, "%s %19s %d %d %d", command, descpription, &month, &day, &hour) != 5)
            {
                printf(">> Virheellinen komento. Tarkista syöte\n");
                break;
            }

            if (check_params(month, day, hour) == 1)
            {
                printf(">> Virheellinen komento. Tarkista syöte\n");
                break;
            }

            calendar = add(descpription, month, day, hour, calendar);
            break;
        // cancel appointment
        case 'd':
            if (tolower((int)input[1]) != ' ')
            {
                printf(">> Virheellinen komento. Tarkista syöte\n");
                break;
            }

            if (sscanf(input, "%s %d %d %d", command, &month, &day, &hour) != 4)
            {
                printf(">> Virheellinen komento. Tarkista syöte\n");
                break;
            }

            if (check_params(month, day, hour) == 1)
            {
                printf(">> Virheellinen komento. Tarkista syöte\n");
                break;
            }
            calendar = cancel(month, day, hour, calendar);
            break;
        // print calendar
        case 'l':
            print_calendar(calendar);
            break;
        //save calendar
        case 'w':
            if (tolower((int)input[1]) != ' ')
            {
                printf(">> Virheellinen komento. Tarkista syöte\n");
                break;
            }

            if (sscanf(input, "%s %19s", command, file_name) != 2)
            {
                printf(">> Virheellinen komento. Tarkista syöte\n");
                break;
            }
            save_calendar(file_name, calendar);
            break;
        // load calendar
        case 'o':
            if (tolower((int)input[1]) != ' ')
            {
                printf(">> Virheellinen komento. Tarkista syöte\n");
                break;
            }

            if (sscanf(input, "%s %19s", command, file_name) != 2)
            {
                printf(">> Virheellinen komento. Tarkista syöte\n");
                break;
            }
            calendar = load_calendar(file_name, calendar);
            break;
        // quit program
        case 'q':
            printf(">> Poistutaan ohjelmasta\n");
            quit_program(calendar);
            return 0;
        // test -- testing quits program in the end
        case 't':
            printf(">> Testataan ohjelmaa...\n");
            test(calendar);
            return 0;
        default:
            printf(">> Tuntematon komento\n");
            break;
        }
    } while (tolower((int)input[0]) != 'q');

    return 0;
}