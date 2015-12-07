#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "defs.h"
#include "structs.h"
#include "data_utility.h"

#define WEEK_WIDTH  10.0f
#define TABLE_WIDTH 100.0f

void print_file_header(FILE *f, char* pageTitle)
{
    fprintf(f, "<html>\n<head>\n");
    fprintf(f, "<title>%s</title>\n", pageTitle);
    fprintf(f, "</head>\n<body>\n");
}

void print_footer(FILE *f)
{
    fprintf(f, "<hr />\nGenerated by <b>AAU Scheduler</b>\n</body>\n</html>\n");
}

void begin_print_table(FILE *f, int cellspacing)
{
    fprintf(f, "<table width=\"%.2f%%\" cellspacing=\"%d\">\n", TABLE_WIDTH, cellspacing);
}

void end_print_table(FILE *f)
{
    fprintf(f, "</table>\n");
}

void print_row_header(FILE *f, double width, const char* str, ...)
{
    va_list pl;
    va_start(pl, str);
    
    fprintf(f, "\t\t<td width=\"%.2f%%\"><b>", width);
    vfprintf(f, str, pl);
    fprintf(f, "</b></td>\n");
    
    va_end(pl);
}

void print_title(FILE *f, const char* title)
{
    fprintf(f, "<h2>Schedule for %s</h2>", title);
}

void begin_print_data(FILE *f, const char* str)
{
    fprintf(f, "\t\t<td>%s", str);
}

void end_print_data(FILE *f)
{
    fprintf(f, "</td>\n");
}

void begin_print_row(FILE *f, const char* backgroundColor)
{
    fprintf(f, "\t<tr bgcolor=\"%s\">\n", backgroundColor);
}

void end_print_row(FILE *f)
{
    fprintf(f, "\t</tr>\n");
}

void print_period(SemesterData *sd, Specialization *sp, FILE *f, int periodId, int weekNumber)
{
    int i, j, k, foundMatches;
    
    begin_print_row(f, "#F0F0F0");
    
    begin_print_data(f, get_name_of_period(periodId));
    end_print_data(f);
    
    /* Print block for all days in this week */
    for (i = weekNumber * DAYS_PER_WEEK; i < (weekNumber + 1) * DAYS_PER_WEEK; i++)
    {
        foundMatches = 0;

        for (j = 0; j < sd->numLectures; j++)
        {
            Lecture *lect = &sd->lectures[j];
            
            /* Validate lecture specialization */
            if (!specialization_has_lecture(sp, lect))
                continue;
            
            /* Validate day and period */
            if (lect->period != periodId || lect->day != i)
                continue;
            
            /* Check if there's already a course on this hour and day*/
            if (foundMatches)
            {
                printf("Warning: Overlap on %s at %s\n",
                    get_name_of_day(lect->day),
                    get_name_of_period(lect->period));
                    
                /* Write regardless */
                fprintf(f, "<br />[+%s]", lect->assignedCourse->name);
            }
            else
            {
                begin_print_data(f, lect->assignedCourse->name);
                fprintf(f, "<b>[lect %d, period %d]</b>", j, lect->day);
                for (k = 0; k < lect->assignedCourse->numTeachers; k++)
                    fprintf(f, "<br />%s", lect->assignedCourse->teachers[k]->name);
                fprintf(f, "<br />%s", lect->assignedRoom->name);
                
                foundMatches++;                
            }
        }
        
        if (!foundMatches)
            begin_print_data(f, "-");

        end_print_data(f);
    }
    
    /* End test row */
    end_print_row(f);
}

/* Prints a schedule for a specific specialization to a file */
void print_schedule_to_file(SemesterData *sd, Specialization *sp, const char* fileName)
{
    int i, j;
    FILE *f;
    f = fopen(fileName, "w");
    
    print_file_header(f, "AAU Scheduler");
    
    print_title(f, sp->name);

    /* Print a table for every week */
    for(i = 0; i < sd->numWeeks; i++)
    {
        begin_print_table(f, 20);
        begin_print_row(f, "#FFFFFF");
        
        print_row_header(f, WEEK_WIDTH, "Week %d", i + 1);
        
        /* Print a row header for every day in the week */
        for (j = 0; j < DAYS_PER_WEEK; j++)
            print_row_header(f, (100.0f - WEEK_WIDTH) / (DAYS_PER_WEEK), "%s", get_name_of_day(j));
        
        end_print_row(f);
        
        /* Print a for with lectures for each period */
        for (j = 0; j < MAX_PERIODS; j++)
            print_period(sd, sp, f, j, i);

        end_print_table(f);    
    }
    
    print_footer(f);
    
    fclose(f);
}