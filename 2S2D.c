#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*Global Variables*/
char *filename;
const char EORCHAR = '|';
const char EOFCHAR = '#';
int whichfiledatastoredin = 0;

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



int markRunsAndDistribute()
{
    int runCount = 0;
    int current, next;
    int flag = 1;

    FILE *f1, *f2;
    FILE* d;

    d = fopen(filename, "r");

    //check if file exists
    if (d == NULL)
    {
        printf("File does not exists %s", "Source.csv");
        return 0;
    }

    f1 = fopen("F1.csv", "w");
    f2 = fopen("F2.csv", "w");
    //read line by line
    const size_t line_size = 2000;
    char currentline[line_size];
    char nextline[line_size];

    printf("\nSTEP 1 : Marking And Distributing Runs : \n\n");
    if (fgets(currentline, line_size, d) != NULL)
    {
        //Stops when the newline character is read or (line_size(read below) - 1 ) characters are read
        while (fgets(nextline, line_size, d) != NULL)
        {
            if (nextline[0] == '#')
            {
                current = returnID(currentline);
                if (flag == 1)
                {
                    fprintf(f1, "%s", currentline);
                    fprintf(f1, "%c\n%c", EORCHAR, EOFCHAR);
                    fprintf(f2, "%c", EOFCHAR);
                    printf("%d |#", current);
                    printf("--S-- ");
                    printf("|#");
                    runCount++;
                    break;
                }
                else
                {
                    fprintf(f2, "%s", currentline);
                    fprintf(f2, "%c\n%c", EORCHAR, EOFCHAR);
                    fprintf(f1, "%c", EOFCHAR);
                    printf("%d |#", current);
                    printf("--S-- ");
                    printf("|#");
                    runCount++;
                    break;
                }
            }
            else //IF NOT END OF FILE
            {
                current = returnID(currentline);
                next = returnID(nextline);
                if(current > next)
                {
                    if (flag == 1)
                    {
                        fprintf(f1, "%s",currentline);
                        fprintf(f1, "%c\n", EORCHAR);
                        printf("%d | ", current);
                        runCount++;
                        flag = 2;
                        printf("--S-- ");
                    }
                    else
                    {
                        fprintf(f2, "%s",currentline);
                        fprintf(f2, "%c\n", EORCHAR);
                        printf("%d | ", current);
                        runCount++;
                        flag = 1;
                        printf("--S-- ");
                    }
                }
                else // CURRENT < NEXT
                {
                    if (flag == 1)
                    {
                        fprintf(f1, "%s", currentline);
                        printf("%d ", current);
                    }
                    else
                    {
                        fprintf(f2, "%s", currentline);
                        printf("%d ", current);
                    }
                }

            }
            strcpy(currentline, nextline);
            memset(nextline, '\0', line_size);
        }
    }
    fclose(d);
    fclose(f1);
    fclose(f2);

    return runCount;
}



//void distributeRuns()
//{
//    printf("\n\nDistributing Runs : ");
//    int flag = 1;
//    char currentline[2000];
//    const int line_size = 2000;
//
//    f1 = fopen("F1.csv", "w");
//    f2 = fopen("F2.csv", "w");
//    d = fopen("F0.csv", "r");
//
//    while (fgets(currentline, line_size, d) != NULL)
//    {
//        if (currentline[0] == EOFCHAR)
//            break;
//
//        if (flag == 1)
//        {
//            fprintf(f1, "%s", currentline);
//            if (currentline[0] == EORCHAR)
//                flag = 2;
//        }
//        else
//        {
//            fprintf(f2, "%s", currentline);
//            if (currentline[0] == EORCHAR)
//                flag = 1;
//        }
//        //Printing Purpose
//        if (currentline[0] == EORCHAR)
//            printf("| Switch | ");
//        else
//            printf("%d ", returnID(currentline));
//
//        memset(currentline, '\0', line_size);
//    }
//    fprintf(f1, "%c", EOFCHAR);
//    fprintf(f2, "%c", EOFCHAR);
//
//    fclose(d);
//    fclose(f1);
//    fclose(f2);
//}



int mergeRuns(int selectedpair)
{
    FILE *d;
    FILE *f1, *f2, *d1, *d2;
    const int line_size = 2000;
    char recordf1[line_size];
    char recordf2[line_size];

    int idf1 = 0; //Initialization doesn't mean much here.
    int idf2 = 0;
    int runCount = 0;

    printf("\n\nMerging Runs : ");

    if (selectedpair == 0)
    {
        f1 = fopen("F1.csv","r");
        f2 = fopen("F2.csv","r");
        d1 = fopen("F3.csv","w");
        d2 = fopen("F4.csv","w");
    }
    else
    {
        f1 = fopen("F3.csv","r");
        f2 = fopen("F4.csv","r");
        d1 = fopen("F1.csv","w");
        d2 = fopen("F2.csv","w");
    }

    d = d1;
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

        printf("--S--");
        if (d == d1)
            d = d2;
        else
            d = d1;
        whichfiledatastoredin = 1 - whichfiledatastoredin;
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

    fprintf(d1, "%c\n", EOFCHAR);
    fprintf(d2, "%c\n", EOFCHAR);
    fclose(f1);
    fclose(f2);
    fclose(d1);
    fclose(d2);
    printf("%c ", EOFCHAR);

    return runCount;
}


int main()
{
    int choice;
    int binarychoiceoffiles = 0;

    printf("\n\t-----FILE SORTING (2S - 2D)-----");
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

    int noofruns = markRunsAndDistribute();
    printf("\n\n******STEP 1 - MARK AND DISTRIBUTE RUNS: No of runs : %d", noofruns);
    if (noofruns != 1)
    {
        int option;
        printf("\n\n\tDo you want to continue (1/0)? ");
        scanf("%d", &option);
        if (option != 1)
            exit(0);

        printf("\n\n******STEP 2 - MERGE RUNS : ");
        noofruns = mergeRuns(binarychoiceoffiles);
        printf("\n\n******AFTER MERGING : %d runs", noofruns);
    }
    else
    {
        binarychoiceoffiles = 1;
        printf("\n\n******STEP 2 - MERGE RUNS ---SKIPPED! ");
    }

    while (noofruns != 1)
    {
        printf("\n\n****** SWAPPING SOURCE AND DESTINATION FILES *****\n\n");
        binarychoiceoffiles = 1 - binarychoiceoffiles;
        noofruns = mergeRuns(binarychoiceoffiles);
        printf("\n\n******AFTER MERGING : %d runs", noofruns);
    }
    printf("\n\n\t-----------SUCCESS : FILE SORTED------------\n\n");

    if (binarychoiceoffiles == 0)
            printf("Sorted Record stored in F3.csv");

    else
            printf("Sorted Record stored in F1.csv");

    return 0;
}
