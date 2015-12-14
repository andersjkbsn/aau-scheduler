/**
 *  \file scheduler.c
 *  \brief The main script of the program, the magic starts here
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "structs.h"
#include "scheduler.h"
#include "config_reader.h"
#include "data_utility.h"
#include "fitness_calculation.h"
#include "defs.h"
#include "html_output.h"
#include "genetic_algorithm.h"

/**
 *  \brief Brief
 *  
 *  \return Returns weather the program has exited with an error or success
 *  
 *  \details Details
 */
int main(void)
{
    char specName[32];
    int i, seed, startTime;
    Generation *gen = 0;
    SemesterData sd;

    /* Null all SemesterData values */
    memset(&sd, 0, sizeof(SemesterData));
 
    /*if (!scanf("%d", &seed))*/
		seed = time(NULL);
    srand(seed);
    
    /* Set start time to now */
    startTime = time(NULL);
    
    /* Read configuration file */
    if (!read_config("scheduler.input", &sd))
    {
        printf("Error: Could not read configuration file.\n");
        exit(1);
    }

    /* Calculate amount of Lectures (genes) */
    calc_amount_of_lectures(&sd);
    
    /* Run genetic algorithm */
    run_ga(&gen, &sd);
    
    /* Print best schedule for each specialization to file */
    for (i = 0; i < sd.numSpecializations; i++)
    {
        sprintf(specName, "%s.html", sd.specializations[i].name);
        
        printf("Writing to file %s..\n", specName);
        print_schedule_to_file(&gen->schedules[0], &sd.specializations[i], specName);
    }
    
    free_semesterdata(&sd);
    free_generation(gen);
    
    printf("Finished, ran for %d seconds.\n", time(NULL) - startTime);
    
    return 0;
}