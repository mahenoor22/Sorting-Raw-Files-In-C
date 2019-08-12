#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*Global Variables*/
FILE *f1, *f2, *d;
char *filename;
const char EORCHAR = '|';
const char EOFCHAR = '#';


//Utility function to return the Primary key of a record
int returnID(char *num)
{
    char buffer[256];
    size_t i;
    for(i = 0; ; ++i)
    {
        if (num[i] == ',')
            break;
        else
            buffer[i] = num[i];
    }
    buffer[i] = '\0';
    int number = atoi(buffer);
    return number;

}



int markRuns()
{
    int runCount = 0;
    int current, next;

    FILE* fh;
    fh = fopen(filename, "r");

    //check if file exists
    if (fh == NULL)
    {
        printf("file does not exists %s", "Source.csv");
        return 0;
    }


    f1 = fopen("F0.csv","w");
    //read line by line
    const size_t line_size = 2000;
    char currentline[2000];
    char nextline[2000];

    printf("\nSTEP 1 : Marking Runs : ");
    if (fgets(currentline, line_size, fh) != NULL)
    {
        //Stops when the newline character is read or (line_size(read below) - 1 ) characters are read
        while (fgets(nextline, line_size, fh) != NULL)
        {
            if (nextline[0] == '#')
            {
                current = returnID(currentline);
                fprintf(f1, "%s", currentline);
                fprintf(f1, "%c\n%c", EORCHAR, EOFCHAR);
                printf("%d |#", current);
                runCount++;
                break;
            }

            else
            {
                current = returnID(currentline);
                next = returnID(nextline);
                if(current > next)
                {
                    fprintf(f1, "%s",currentline);
                    fprintf(f1, "%c\n", EORCHAR);
                    printf("%d | ", current);
                    runCount++;
                }
                else
                {
                    fprintf(f1, "%s", currentline);
                    printf("%d ", current);
                }

            }
            strcpy(currentline, nextline);

        }
    }
    fclose(fh);
    fclose(d);
    fclose(f1);

    return runCount;
}



void distributeRuns()
{
    printf("\n\nDistributing Runs : ");
    int flag = 1;
    char currentline[2000];
    const int line_size = 2000;

    f1 = fopen("F1.csv", "w");
    f2 = fopen("F2.csv", "w");
    d = fopen("F0.csv", "r");

    while (fgets(currentline, line_size, d) != NULL)
    {
        if (currentline[0] == EOFCHAR)
            break;

        if (flag == 1)
        {
            fprintf(f1, "%s", currentline);
            if (currentline[0] == EORCHAR)
                flag = 2;
        }
        else
        {
            fprintf(f2, "%s", currentline);
            if (currentline[0] == EORCHAR)
                flag = 1;
        }
        //Printing Purpose
        if (currentline[0] == EORCHAR)
            printf("| Switch | ");
        else
            printf("%d ", returnID(currentline));

        memset(currentline, '\0', line_size);
    }
    fprintf(f1, "%c", EOFCHAR);
    fprintf(f2, "%c", EOFCHAR);

    fclose(d);
    fclose(f1);
    fclose(f2);
}



int mergeRuns()
{
    printf("\n\nMerging Runs : ");
    char recordf1[2000];
    char recordf2[2000];
    int idf1 = 0; //Initialization doesn't mean much here.
    int idf2 = 0;
    int runCount = 0;
    int line_size = 2000;

    f1 = fopen("F1.csv","r");
    f2 = fopen("F2.csv","r");
    d = fopen("F0.csv","w");

    while ((fgets(recordf1, line_size, f1) != NULL) && (fgets(recordf2, line_size, f2) != NULL))
    {
        if ((recordf1[0] == EOFCHAR) || (recordf2[0] == EOFCHAR))
            break;

        while ((recordf1[0] != EORCHAR) && (recordf2[0] != EORCHAR))
        {
            idf1 = returnID(recordf1);
            idf2 = returnID(recordf2);
            if (idf1 < idf2)
            {
                printf("%d ", idf1);
                fprintf(d, "%s", recordf1);
                memset(recordf1, '\0', line_size);
                fgets(recordf1, line_size, f1);
            }
            else
            {
                printf("%d ", idf2);
                fprintf(d, "%s", recordf2);
                memset(recordf2, '\0', line_size);
                fgets(recordf2, line_size, f2);
            }
        }

        if (recordf1[0] == EORCHAR)
        {
            while (recordf2[0] != EORCHAR)
            {
                fprintf(d, "%s", recordf2);
                printf("%d ", returnID(recordf2));
                memset(recordf2, '\0', line_size);
                fgets(recordf2, line_size, f2);
            }
        }
        else if (recordf2[0] == EORCHAR)
        {
            while (recordf1[0] != EORCHAR)
            {
                fprintf(d, "%s", recordf1);
                printf("%d ", returnID(recordf1));
                memset(recordf1, '\0', line_size);
                fgets(recordf1, line_size, f1);
            }
        }
        printf("%c ", EORCHAR);
        fprintf(d, "%c\n", EORCHAR);
        runCount++;
    }

    if (recordf1[0] == EOFCHAR)
    {
        while (recordf2[0] != EOFCHAR)
        {
            if (recordf2[0] == EORCHAR)
                runCount++;
            fprintf(d, "%s", recordf2);

            //Printing
            if (recordf2[0] != EORCHAR)
                printf("%d ", returnID(recordf2));
            else
                printf("%c ", EORCHAR);

            memset(recordf2, '\0', line_size);
            fgets(recordf2, line_size, f2);
        }
    }
    else if (recordf2[0] == EOFCHAR)
    {
        while (recordf1[0] != EOFCHAR)
        {
            if (recordf1[0] == EORCHAR)
                runCount++;

            //Printing
            if (recordf1[0] != EORCHAR)
                printf("%d ", returnID(recordf1));
            else
                printf("%c ", EORCHAR);

            fprintf(d, "%s", recordf1);
            memset(recordf1, '\0', line_size);
            fgets(recordf1, line_size, f1);
        }
    }
    //REMEMBER memset(currentline, '\0', line_size);

    fprintf(d,"%c\n",EOFCHAR);
    fclose(f1);
    fclose(f2);
    fclose(d);

    printf("%c ", EOFCHAR);

    return runCount;
}


int main()
{
    int choice;
    printf("\n\t-----FILE SORTING (2S - 1D)-----");
    printf("\n\t1. Sorted.");
    printf("\n\t2. Reverse.");
    printf("\n\t3. Random.");
    printf("\n\t4. Sorted(except for one record).");
    printf("\n\t5. Reverse(except for one record).");
    printf("\n\n\tEnter the Data Type : ");
    scanf("%d", &choice);
    switch(choice)
    {
    case 1:
        filename = "Sorted.csv";
        break;
    case 2:
        filename = "Reverse.csv";
        break;
    case 3:
        filename = "Random.csv";
        break;
    case 4:
        filename = "SortedButOne.csv";
        break;
    case 5:
        filename = "ReverseButOne.csv";
        break;
    default:
        exit(-1);
    }

    int noofruns = markRuns();
    printf("\n\n******STEP 1 - MARK RUNS: No of runs : %d", noofruns);
    if (noofruns != 1)
    {
        int option;
        printf("\n\n\tDo you want to continue (1/0)? ");
        scanf("%d", &option);
        if (option != 1)
            exit(0);

        printf("\n\n******STEP 2 - DISTRIBUTE : ");
        distributeRuns();
        printf("\n\n******STEP 3 - MERGE AND DISTRIBUTE : ");
        noofruns = mergeRuns();
        printf("\n\n******AFTER MERGING : %d runs", noofruns);
    }
    else
    {
        printf("\n\n******STEP 2 - DISTRIBUTE ---SKIPPED! ");
        printf("\n\n******STEP 3 - MERGE and DISTRIBUTE ---SKIPPED! ");

    }

    while (noofruns != 1)
    {
        distributeRuns();
        noofruns = mergeRuns();
        printf("\n\n******AFTER MERGING : %d runs", noofruns);
    }
    printf("\n\n\t-----------SUCCESS : FILE SORTED------------\n\n");
    return 0;
}




