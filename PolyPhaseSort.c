#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*Global Variables*/
FILE *d;
FILE *source, *f1, *f2, *f3;
char *filename;
const char EORCHAR = '|';
const char EOFCHAR = '#';


//Utility function to return the Primary key of a record - Not applicable for non .csv files.
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


//Similar Methods of 2S1D and 2S2D


int mergeRuns(char *file1name, char *file2name, char *destname)
{
    printf("\n\nMerged Runs : ");
    char recordf1[2000];
    char recordf2[2000];
    int idf1 = 0; //Initialization doesn't mean much here.
    int idf2 = 0;
    int runCount = 0;
    int line_size = 2000;

    FILE *f1 = fopen(file1name,"r");
    FILE *f2 = fopen(file2name,"r");
    FILE *d = fopen(destname,"w");

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
    printf("%c ", EOFCHAR);
    int mergedruns = runCount;
    printf("\nNo. of Runs Merged : %d", runCount);
    printf("\nLeftover Runs : ");
    if (recordf1[0] == EOFCHAR)
    {
        fclose(f1);
        f1 = fopen(file1name, "w");
        while (recordf2[0] != EOFCHAR)
        {
            if (recordf2[0] == EORCHAR)
                runCount++;
            fprintf(f1, "%s", recordf2);

            //Printing
            if (recordf2[0] != EORCHAR)
                printf("%d ", returnID(recordf2));
            else
                printf("%c ", EORCHAR);

            memset(recordf2, '\0', line_size);
            fgets(recordf2, line_size, f2);
        }
        fprintf(f1, "%c\n", EOFCHAR);

        fclose(f2);
        f2 = fopen(file2name, "w");
        fprintf(f2, "%c\n", EOFCHAR);
    }
    else if (recordf2[0] == EOFCHAR)
    {
        fclose(f2);
        f2 = fopen(file2name, "w");
        while (recordf1[0] != EOFCHAR)
        {
            if (recordf1[0] == EORCHAR)
                runCount++;

            //Printing
            if (recordf1[0] != EORCHAR)
                printf("%d ", returnID(recordf1));
            else
                printf("%c ", EORCHAR);

            fprintf(f2, "%s", recordf1);
            memset(recordf1, '\0', line_size);
            fgets(recordf1, line_size, f1);
        }
        fprintf(f2, "%c\n", EOFCHAR);

        fclose(f1);
        f1 = fopen(file1name, "w");
        fprintf(f1, "%c\n", EOFCHAR);
    }
    printf("%c ", EOFCHAR);
    //REMEMBER memset(currentline, '\0', line_size);
    printf("\nNo. of Leftover runs : %d", runCount - mergedruns);
    fprintf(d, "%c\n", EOFCHAR);
    fclose(f1);
    fclose(f2);
    fclose(d);

    //printf("%c ", EOFCHAR);
    return runCount;
}


/*NOTE: copyRunsToFile need opened source and destination files. The file also needs to be rewinded. That is, the
read pointer should be at the start.*/
void copyRunsToFile(FILE *source, FILE *dest, int noofruns, int skipruns)
{
    const size_t line_size = 2000;
    char currentline[line_size];

    int index = 1;

    //Continue till all noofruns are copied into f1 from source.
    while (index <= skipruns && fgets(currentline, line_size, source) != NULL)
    {
        if (currentline[0] != '#')
        {
            if (currentline[0] == '|')
                index++;
        }
        else
        {
            printf("Error: Not enough number of runs or # misplaced.");
            break;
        }
    }

    index = 1;

    while (fgets(currentline, line_size, source) != NULL && index <= noofruns)
    {
        if (currentline[0] != '#')
            fprintf(dest, "%s", currentline);
        else
        {
            printf("Error: Not enough number of runs or # misplaced.");
            break;
        }
        if (currentline[0] == '|') // if it's an end of record marker increment 'index' count.
            index++;
    }
    fprintf(dest, "#");
    rewind(source);
    if (index == noofruns + 1)
        printf("\n\tLOG: File : Runs Copied Successfully.");
}

void appendSortedAndLeftOverRunsToOneFile(char *sortedfilename, bool leftoverrunspresent)
{
    FILE *sorted, *leftover;
    FILE *dest;

    sorted = fopen(sortedfilename, "r");
    leftover = fopen("leftover.csv", "r");
    dest = fopen("F0.csv", "w");

    //Appending leftover file(IF PRESENT) first to F0.csv
    const size_t line_size = 2000;
    char currentline[line_size];

    if (leftoverrunspresent)
    {
        while (fgets(currentline, line_size, leftover) != NULL)
        {
            if (currentline[0] != '#')
                fprintf(dest, "%s", currentline);
        }
    }

    //---------------------------------------
    //Appending sorted file to F0.csv
    while (fgets(currentline, line_size, sorted) != NULL)
    {
        fprintf(dest, "%s", currentline);
    }
    printf("\n\tLOG: Sorted File merged with the leftover runs.");
    fclose(sorted);
    fclose(leftover);
    fclose(dest);
}

void sortFibonacciNumberFile(int fibo1, int fibo2, bool leftoverrunspresent)
{
    source = fopen("F0.csv", "r");
    FILE *f[3];
    char *filenames[] = {"File0.csv", "File1.csv", "File2.csv"};


    f[0] = fopen(filenames[0], "w");
    f[1] = fopen(filenames[1], "w");
    f[2] = fopen(filenames[2], "w");


    //Write 'f1' number of runs to one of the files
    copyRunsToFile(source, f[0], fibo1, 0);

    //Write 'f2' number of runs to the other file
    copyRunsToFile(source, f[1], fibo2, fibo1);


    fclose(source);
    fclose(f[0]);
    fclose(f[1]);
    fclose(f[2]);

    //char dummy;
    bool switchflag = true;

    int noofruns;
    do
    {
        if (switchflag) //First destination is the third
            noofruns = mergeRuns(filenames[0], filenames[1], filenames[2]);
        else
            noofruns = mergeRuns(filenames[2], filenames[1], filenames[0]);

        switchflag = !switchflag;
    }
    while (noofruns != 1);

    if (switchflag)
    {
        //Append using filename[0] because the switch flag was inverted when the loop was exited.
        //So the sorted run is in filename[0]
        appendSortedAndLeftOverRunsToOneFile(filenames[0], leftoverrunspresent);
    }
    else
    {
        appendSortedAndLeftOverRunsToOneFile(filenames[2], leftoverrunspresent);
    }
}




void  writeExtraRunsToTempFile(int nofruns, int endofruns)
{
    FILE *f;
    FILE *dest;
    FILE *leftover;

    //const size_t line_size = 2000;
    //char currentline[line_size];

    f = fopen("F0.csv", "r");
    dest = fopen("temp.csv", "w");
    leftover = fopen("leftover.csv", "w");

    rewind(f);
    copyRunsToFile(f, dest, nofruns, 0);
    rewind(f);
    copyRunsToFile(f, leftover, endofruns - nofruns, nofruns);
    fclose(f);
    fclose(dest);
    fclose(leftover);
    printf("\n\tLOG: Extra Runs removed from file.");

    if (remove("F0.csv") != 0)
        printf("\nERROR : Unable to delete the file");


    if(rename("temp.csv", "F0.csv") != 0)
        printf("\nERROR: Unable to rename the file");
}


//Fast_Fib
void fast_fib(int n, int ans[])
{
    if(n == 0)
    {
        ans[0] = 0;
        ans[1] = 1;
        return;
    }
    int fnby2, fnby2plus1, fn, fnplus1;

    fast_fib((n / 2), ans);
    fnby2 = ans[0];             /* F(n) */
    fnby2plus1 = ans[1];             /* F(n+1) */

    fn = (fnby2 * (2 * fnby2plus1 - fnby2));      /* F(2n) */
    fnplus1 = ((fnby2 * fnby2) + (fnby2plus1 * fnby2plus1));  /* F(2n + 1) = square(fn) + square(fn+1)*/

    /*In fibonacci a pair is between a (even, odd) number where
    even < odd. We need to maintain this starting from f(0), f(1)*/

    if(n % 2 == 0) // Even f(2) called f(1) | Even f(10) called f(5)
    {
        ans[0] = fn; //f(2) | f(10)
        ans[1] = fnplus1; // f(3) | f(11)
    }
    else // Odd f(1) called f(0) | Odd f(5) called f(2)
    {
        ans[0] = fnplus1; // f(1) | f(5)

        /*IMPORTANT : If n is odd then we need to
        it's corresponding*/
        //f(2n + 2) down
        ans[1] = fn + fnplus1;  // f(2) | f(6)
    }
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
    char currentline[line_size];
    char nextline[line_size];

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



int main()
{
    int choice;
    printf("\n\t-----POLYPHASE SORTING-----");
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

    while (noofruns != 1)
    {
        printf("\n--------------------------------------------------------------------------");
        int indexoffib = 2;
        int justgreaterthannoofruns[2];
        int fibonaccinumbertoprocess;

        do
        {
            fast_fib(indexoffib++, justgreaterthannoofruns);
        }
        while (justgreaterthannoofruns[1] < noofruns);

        //if the no of runs is a perfect fibonacci number
        if (justgreaterthannoofruns[1] == noofruns)
            fibonaccinumbertoprocess = justgreaterthannoofruns[1];
        else // if it's not.
            fibonaccinumbertoprocess = justgreaterthannoofruns[0];

        printf("\n\nNo of runs : %d", noofruns);
        printf("\nThe perfect Fibonacci number here to process : %d", fibonaccinumbertoprocess);

        int extraruns = noofruns - fibonaccinumbertoprocess;
        printf("\nExtra Runs : %d", extraruns);

        //If there are extra runs, then write them extra runs to a temporary file
        if (noofruns != fibonaccinumbertoprocess)
        {
            writeExtraRunsToTempFile(fibonaccinumbertoprocess, noofruns);
            if (justgreaterthannoofruns[0] != 0) // This condition is a little doubtful to ever NOT happen
            {
                int fibo1 = justgreaterthannoofruns[1] - justgreaterthannoofruns[0];
                int fibo2 = justgreaterthannoofruns[0] - fibo1;
                printf("\nFibo 1 : %d", fibo1);
                printf("\nFibo 2 : %d", fibo2);
                sortFibonacciNumberFile(fibo1, fibo2, true);
            }
            else // will this ever execute? NO
            {
                printf("ERROR: No fibonacci number exists below 0.");
            }
            //sortFibonacciNumberFile(13, 8);
        }

        //If there are no extra runs, then proceed without writing them.
        else
        {
            if (justgreaterthannoofruns[0] != 0) //Again, This condition is a little doubtful to ever not happen
            {
                int fibo1 = justgreaterthannoofruns[0];
                int fibo2 = justgreaterthannoofruns[1] - justgreaterthannoofruns[0];
                printf("\nFibo 1 : %d", fibo1);
                printf("\nFibo 2 : %d", fibo2);
                sortFibonacciNumberFile(fibo1, fibo2, false);
            }
            else
            {
                printf("ERROR: No fibonacci number exists below 0.");
            }
        }
        noofruns = extraruns + 1;
        printf("\nAfter this phase, the no. of runs : %d", noofruns);
    }
    printf("\n\n\t-----------SUCCESS : FILE SORTED------------\n\n");
    return 0;
}
