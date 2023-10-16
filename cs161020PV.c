/*********************************************************
 *                     ΠΑΟΛΑ ΒΕΛΑΣΚΟ                  
 *                   cs161020, 9ο εξάμηνο 
 * *******************************************************
 *              Πανεπιστήμιο Δυτικής Αττικής 
 *       Τμήμα Μηχανικών Πληροφορικής και Υπολογιστών
 * -------------------------------------------------------
 *            Εισαγωγή στον Παράλληλο Υπολογισμό 
 *            Εργαστήριο Τμήμα: Ε2 (Δευτέρα 1-2)
 *        Καθηγητές: κος Β. Μάμαλης, κος Μ. Ιορδανάκης
 * -------------------------------------------------------
 *        ΕΡΓΑΣΙΑ 2 - MPI in C (Συλλογική Επικοινωνία)
 *********************************************************/

#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>

int main_menu();
void initArrayX(int arrayX[], int arrayX_size);
int menu();
void greaterLess(int countGreater, int countLess, int sendCounts[], int my_rank, int localArrayX[], float avg, int *totalCountGreater, int root, int *totalCountLess);
void var(float numerator, int sendCounts[], int my_rank, int localArrayX[], float avg, float *totalNumerators, int root, int arrayX_size);

void freeSpace(double arrayD[], int arrayX[], double localArrayD[], int localArrayX[], int sendCounts[], int displacements[], int my_rank);

struct info
{
    float value;
    int index;
};

/*global variables in, out type struct info*/
struct info inmin;
struct info outmin;
struct info inmax;
struct info outmax;

int main(int argc, char *argv[])
{
    int numtasks; // Αριθμός των διεργασιών
    int my_rank;  // Η ταυτότητα της διεργασίας
    int root = 0; // Η ρίζα

    int i;
    int rc;

    int choice_mainmenu;
    int choice;

    int *arrayX;     // Αρχικός πίνακας Χ
    int arrayX_size; // μήκος του αρχικού πίνακα Χ

    int *localArrayX;   // Τοπικός πίνακας που θα έχει στοιχεία όσα του ανατεθούν
    int *sendCounts;    // Πίνακας που περιέχει το μήκος των πινάκων κάθε διεργασίας
    int *displacements; // Πίνακας offset

    float sum;
    float tot_sum;
    float avg; // Final average result

    int local_min; // Ελάχιστη τιμή στον τοπικό πίνακα κάθε διεργασίας
    int local_max; // Μέγιστη τιμή στον τοπικό πίνακα κάθε διεργασίας
    int min;       // Ελάχιστη τιμή στον πίνακα Χ
    int max;       // Μέγιστη τιμή στον πίνακα Χ

    int countGreater;      // a counter that keeps records of how many elements are greater than the average's value in each process
    int countLess;         // a counter that keeps records of how many elements are less than the average's value in each process
    int totalCountGreater; // Τελικό αποτέλεσμα για πόσες τιμές είναι μεγελύτερες από τη μέση τιμή
    int totalCountLess;    // Τελικό αποτέλεσμα για πόσες τιμές είναι μικρότερες από τη μέση τιμή

    float numerator;       // Υπολογισμός αριθμητή της κάθε διεργάσιας για τη διασπορά
    float totalNumerators; // Συνολικός αριθμητής για τη διασπορά
    float totalVar;        // Διασπορά του πίνακα Χ

    double *arrayD;      // Πίνακας arrayD
    double *localArrayD; // Τοπικός πίνακας arrayD

    int minrank, minindex;
    float minval;

    int maxrank, maxindex;
    float maxval;

    /************************************************ΠΡΟΕΤΟΙΜΑΣΙΑ ΠΡΟΓΡΑΜΜΑΤΟΣ***********************************************/

    // start the parallelism
    rc = MPI_Init(&argc, &argv);
    if (rc != 0)
    {
        printf("MPI initialization error\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }
    // function that returns the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    // function that returns the rank of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Main menu loop
    do
    {

        if (my_rank == 0)
        {
            choice_mainmenu = main_menu(); //choose an operation from the main menu
        }

        // Broadcast επιλογή από την main menu
        MPI_Bcast(&choice_mainmenu, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Εισαγωγή νέων δεδομένων
        if (choice_mainmenu == 1)
        {

            // Μήκος του πίνακα από το πληκτρολόγιο
            if (my_rank == root)
            {
                printf("Give length of array X: ");
                scanf("%d", &arrayX_size);
            }

            // Αποστολή το μήκος του πίνακα με MPI_Bcast στους υπόλοιπους επεξεργαστές
            MPI_Bcast(&arrayX_size, 1, MPI_INT, root, MPI_COMM_WORLD);

            // Υπολογισμός: πόσα νούμερα θα λάβει η κάθε διεργασία

            // Create an array that keeps how many numbers should be given to each process
            sendCounts = (int *)malloc(sizeof(int) * numtasks);
            // Check if malloc was successfully completed
            if (!sendCounts)
            {
                printf("Error: Not available memory\n");
                exit(EXIT_FAILURE);
            }

            // dividing how many numbers should be given to each process & save it to array sendCounts[]
            int mod;
            mod = arrayX_size % numtasks;
            for (i = 0; i < numtasks; i++)
            {

                if (i < mod)
                {
                    *(sendCounts + i) = arrayX_size / numtasks + 1;
                }
                else
                {
                    *(sendCounts + i) = arrayX_size / numtasks;
                }
            }

            /*debugging: sendCounts[] 
            if (my_rank == 0)
            {
                for (i = 0; i < numtasks; i++)
                {
                    printf("sendCounts[%d] = %d\n", i, sendCounts[i]);
                }
            }
            */

            // Create an array that keeps the offsets
            displacements = (int *)malloc(sizeof(int) * numtasks);
            // Check if malloc was successfully completed
            if (!displacements)
            {
                printf("Failure");
                exit(0);
            }

            // Αρχικοποίηση του πίνακα displacements
            int index = 0;
            for (i = 0; i < numtasks; i++)
            {

                if (i == 0)
                {
                    displacements[i] = index;
                }
                else
                {
                    index += *(sendCounts + (i - 1));
                    displacements[i] = index;
                }
            }

            /*debugging: displacements[] 
            if (my_rank == 0)
            {
                for (i = 0; i < numtasks; i++)
                {
                    printf("displacements[%d] = %d\n", i, displacements[i]);
                }
            }
            */

            // allocate memory for localArrayX
            localArrayX = (int *)malloc(sizeof(int) * sendCounts[my_rank]);
            // Check if malloc was successfully completed
            if (localArrayX == NULL)
            {
                printf("Error: Not available memory\n");
                exit(EXIT_FAILURE);
            }

            /****************************************ΤΕΛΟΣ ΠΡΟΕΤΟΙΜΑΣΙΑ ΠΡΟΓΡΑΜΜΑΤΟΣ***********************************************/

            /* Δημιουργία πίνακα Χ στη διεργασί 0 & αρχικοποίησή του με τιμές από πληκτρολόγιο */
            if (my_rank == 0)
            {
                // allocate memory for array X
                arrayX = (int *)malloc(sizeof(int) * arrayX_size); // returns the adress that memory has provided to the array
                // Check if malloc was successfully completed
                if (arrayX == NULL)
                {
                    printf("Error: Not available memory\n");
                    exit(EXIT_FAILURE);
                }

                // Call function initArrayX to initialize arrayX
                initArrayX(arrayX, arrayX_size);

                /* debugging: print array X
                printf("\nHello I am process: %d ", my_rank);
                printf("Contents of array X\n");
                for (i = 0; i < arrayX_size; i++)
                {
                    printf("arrayX[%d] = %d\n", i, arrayX[i]);
                }
                */
            }

            // Διαμοιρασμός πίνακα arrayX  και στέλνονται οι κατάλληλοι αριθμοί σε κάθε διεργασία
            MPI_Scatterv(arrayX, sendCounts, displacements, MPI_INT, localArrayX, sendCounts[my_rank], MPI_INT, root, MPI_COMM_WORLD);
            // printf(" Hello I am process %d and I have received number = %d", my_rank, number);

            /* debugging: local array X
            printf("\nHello I am process: %d\nContents of local array X\n", sendCounts[my_rank]);
            for (i = 0; i < sendCounts[my_rank]; i++)
            {
                printf("localArrayX[%d] = %d\n", i, localArrayX[i]);
            }*/

            /***********************************************************************/
            /*                      Υπολογισμός Μέσης Τιμής                        */

            sum = 0;

            for (i = 0; i < sendCounts[my_rank]; i++)
            {
                sum += localArrayX[i];
            }

            // Reducte & MPI_SUM the results of each process & calculate the average
            MPI_Reduce(&sum, &tot_sum, 1, MPI_FLOAT, MPI_SUM, root, MPI_COMM_WORLD);
            if (my_rank == root)
            {
                avg = tot_sum / (float)arrayX_size;
                printf("The avg of all elements equalts to, avg=%.2f\n", avg);
            }

            // Αποστολή από την ρίζα την τιμή της μέσης τιμής
            MPI_Bcast(&avg, 1, MPI_FLOAT, root, MPI_COMM_WORLD);

            /* debugging: Αν έχουν λάβει όλοι την τιμή του ΜΟ
            printf("Hello, I am process: %d and I have received from %d the value of avg = %.2f\n", my_rank, root, avg);
            */

            /***********************************************************************/
            /*                              MIN & MAX                             */

            // find min & max of localArrayX of each process
            local_min = localArrayX[0];
            local_max = localArrayX[0];

            for (i = 0; i < sendCounts[my_rank]; i++)
            {
                if (localArrayX[i] < local_min)
                {
                    local_min = localArrayX[i];
                }

                if (localArrayX[i] > local_max)
                {
                    local_max = localArrayX[i];
                }
            }

            // REDUCE & MPI_MIN: Find the minimum and send it to root
            MPI_Reduce(&local_min, &min, 1, MPI_INT, MPI_MIN, root, MPI_COMM_WORLD);

            // REDUCE & MPI_MAX: Find the maximum and send it to root
            MPI_Reduce(&local_max, &max, 1, MPI_INT, MPI_MAX, root, MPI_COMM_WORLD);

            if (my_rank == root)
            {
                printf("The minimum element value in array X is: %d\n", min);
                printf("The maximum element value in array X is: %d\n", max);
            }

            // Αποστολή από την ρίζα τις τιμές min & max
            MPI_Bcast(&min, 1, MPI_FLOAT, root, MPI_COMM_WORLD);
            MPI_Bcast(&max, 1, MPI_FLOAT, root, MPI_COMM_WORLD);

            /* debugging: Αν έχουν λάβει όλοι την τιμή του ΜΟ
            printf("Hello, I am process: %d and I have received from %d the value of min = %d\n", my_rank, root, min);
            printf("Hello, I am process: %d and I have received from %d the value of max = %d\n", my_rank, root, max);
            */

            // Inner menu loop of main menu: για τα υποερωτήματα της άσκησης
            int flag = 1;
            do
            {
                if (my_rank == 0)
                {
                    choice = menu(); //choose an operation from the menu (για τα υποερωτήματα της άσκησης)
                }

                // Broadcast την επιλογή μενού για τα υποερωτήματα της άσκησης
                MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

                if (choice == 1) // ΕΡΩΤΗΣΗ Α
                {
                    greaterLess(countGreater, countLess, sendCounts, my_rank, localArrayX, avg, &totalCountGreater, root, &totalCountLess);

                    if (my_rank == 0)
                    {
                        printf("Total elements that are greater than the average value: %d\n", totalCountGreater);
                        printf("Total elements that are less than the average value: %d\n", totalCountLess);
                    }
                }
                else if (choice == 2) // ΕΡΩΤΗΣΗ Β
                {

                    var(numerator, sendCounts, my_rank, localArrayX, avg, &totalNumerators, root, arrayX_size);
                    if (my_rank == 0)
                    {

                        totalVar = totalNumerators / arrayX_size;

                        printf("Total var of elements: var = %.2f\n", totalVar);
                    }
                }
                else if (choice == 3) // ΕΡΩΤΗΣΗ Γ
                {
                    // Allocate memory for localArrayD
                    localArrayD = (double *)malloc(sizeof(double) * sendCounts[my_rank]);
                    //Check if malloc was executed successfully
                    if (localArrayD == NULL)
                    {
                        printf("Error: Not available memory\n");
                        exit(EXIT_FAILURE);
                    }

                    // Αρχικοποίηση πίνακα localArrayX της κάθε διεργασίας
                    for (i = 0; i < sendCounts[my_rank]; i++)
                    {
                        localArrayD[i] = ((localArrayX[i] - min) / (double)(max - min)) * 100.0;
                        //printf("\nI am process %d with localArrayD[%d] = %.15f\n", my_rank, i, localArrayD[i]);
                    }

                    if (my_rank == root)
                    {
                        // Allocate memory for arrayD
                        arrayD = (double *)malloc(sizeof(double) * arrayX_size);
                        //Check if malloc was executed successfully
                        if (arrayD == NULL)
                        {
                            printf("Error: Not available memory\n");
                            exit(EXIT_FAILURE);
                        }
                    }

                    // Send localArrayD of each process to the root and store it in array arrayD
                    MPI_Gatherv(localArrayD, sendCounts[my_rank], MPI_DOUBLE, arrayD, sendCounts, displacements, MPI_DOUBLE, root, MPI_COMM_WORLD);

                    // Εκτύπωση πίνακα arrayD από την ρίζα
                    if (my_rank == root)
                    {
                        for (i = 0; i < arrayX_size; i++)
                        {
                            printf("arrayD[%d] = %.15f\n", i, arrayD[i]);
                        }
                    }
                }
                else if (choice == 4) // ΕΡΩΤΗΣΗ Δ
                {
                    if (localArrayD == NULL)
                    {
                        if (my_rank == 0)
                            printf("Question 3 should be first executed\n");
                    }
                    else
                    {

                        inmin.value = localArrayD[0];
                        inmax.value = localArrayD[0];
                        inmin.index = 0;
                        inmax.index = 0;

                        // Υπολογισμός & εύρεση θέσης μικρότερης & μεγαλύτερης τιμής σε κάθε τοπικό πίνακα Δ της κάθε διεργασίας
                        for (i = 1; i < sendCounts[my_rank]; i++)
                        {
                            if (inmin.value > localArrayD[i])
                            {
                                inmin.value = localArrayD[i];
                                inmin.index = i;
                            }

                            if (inmax.value < localArrayD[i])
                            {
                                inmax.value = localArrayD[i];
                                inmax.index = i;
                            }
                        }

                        inmin.index = my_rank * sendCounts[my_rank] + inmin.index;
                        inmax.index = my_rank * sendCounts[my_rank] + inmax.index;

                        MPI_Reduce(&inmin, &outmin, 1, MPI_FLOAT_INT, MPI_MINLOC, root, MPI_COMM_WORLD);
                        MPI_Reduce(&inmax, &outmax, 1, MPI_FLOAT_INT, MPI_MAXLOC, root, MPI_COMM_WORLD);

                        if (my_rank == root)
                        {
                            minval = outmin.value;
                            minrank = outmin.index / sendCounts[my_rank];
                            minindex = outmin.index % sendCounts[my_rank];

                            int globalindexmin = minrank * sendCounts[my_rank] + minindex;

                            maxval = outmax.value;
                            maxrank = outmax.index / sendCounts[my_rank];
                            maxindex = outmax.index % sendCounts[my_rank];

                            int globalindexmax = maxrank * sendCounts[my_rank] + maxindex;

                            printf("The min val is %f\n", minval);
                            printf("The rank that has the min val is rank = %d\n", minrank);
                            printf("The index is at %d\n", minindex);
                            printf("The global index is at %d\n", globalindexmin);

                            printf("The max val is %f\n", maxval);
                            printf("The rank that has the max val is rank = %d\n", maxrank);
                            printf("The index is at %d\n", maxindex);
                            printf("The global index is at %d\n", globalindexmax);
                        }
                    }
                }
                else if (choice == 5)
                {
                    // Call function freeSpace to deallocate memory
                    freeSpace(arrayD, arrayX, localArrayD, localArrayX, sendCounts, displacements, my_rank);

                    localArrayD = NULL;

                    flag = 0;
                }
                else if (choice == 6)
                {
                    exit(0);
                }
                else
                {
                    if (my_rank == 0)
                        printf("WRONG CHOICE\n");
                }

            } while (flag);
        }
        else if (choice_mainmenu == 2) // ΕΠΙΛΟΓΗ 2 από την main menu
        {
            if (my_rank == 0)
            {
                printf("\ncs161020\nPaola Velasco\nSemester 9th\n");
            }
        }
        else if (choice_mainmenu == 3) // ΕΠΙΛΟΓΗ 3 από την main menu
        {
            if (my_rank == 0)
            {
                printf("Exiting Program...\n");
                exit(0);
            }
        }
    } while (1);

    // end of parallelism
    MPI_Finalize();

    return 0;
}

// Συνάρτηση: εμφάνιση main menu & επιστροφή της τιμής της επιλογής
int main_menu()
{
    int choice_mainmenu;
    while (1)
    {
        printf("\n\nProgramme Main Menu\n");
        printf("1.) Εισαγωγή νέων δεδομένων \n");
        printf("2.) Εμφάνιση στοιχείων της φοιτήτριας\n");
        printf("3.) Exit\n");

        printf("\nEnter Your Menu Choice: ");
        scanf("%d", &choice_mainmenu);
        switch (choice_mainmenu)
        {

        case 1:
            printf("\nΕπιλογή 1\n\n");
            return 1;
            break;

        case 2:
            printf("\nΕπιλογή 2\n");
            return 2;
            break;

        case 3:
            printf("\nΕπιλογή 3\n");
            return 3;
            break;

        default:
            printf("\nThis is not a valid Menu Option! Please Select Another\n\n");
            break;
        }
    }
}

// Συνάρτηση: αρχικοποίηση αρχικού πίνακα Χ
void initArrayX(int arrayX[], int arrayX_size)
{
    int i;
    // initialize array X
    printf("\nGive numbers for arrayX\n");
    for (i = 0; i < arrayX_size; i++)
    {
        printf("arrayX[%d]: ", i);
        scanf("%d", &arrayX[i]);
    }
}

// Συνάρτηση: εμφάνιση menu υποερωτήματα της άσκησης & επιστροφή της τιμής της επιλογής
int menu()
{
    int choice;
    while (1)
    {
        printf("\n\nProgramme Main Menu\n");
        printf("1.) Πόσα στοιχεία του Χ έχουν μικρότερη και πόσα μεγαλύτερη τιμή από τη μέση τιμή; \n");
        printf("2.) Διασπορά των στοιχείων του διανύσματος Χ\n");
        printf("3.) Υπολόγισε τη ποσοστιάια σχέση των στοιχείων του Χ με τη διαφορά μεγίστου-ελαχίστου\n");
        printf("4.) Ποιά είναι η μεγαλύτερη τιμη του διανύσματος Δ και για ποιό στοιχείο\n");
        printf("5.) Go back to main menu\n");
        printf("6.) Exit\n");

        printf("\nEnter Your Menu Choice: ");
        scanf("%d", &choice);
        switch (choice)
        {

        case 1:
            printf("\nChoice 1\n");
            return 1;
            break;

        case 2:
            printf("\nChoice 2\n");
            return 2;
            break;

        case 3:
            printf("\nChoice 3\n");
            return 3;
            break;

        case 4:
            printf("\nChoice 4\n");
            return 4;
            break;

        case 5:
            printf("\nChoice 5\n");
            return 5;
            break;

        case 6:
            printf("\nChoice 6\n");
            return 6;
            break;

        default:
            printf("\nThis is not a valid Menu Option! Please Select Another\n\n");
            break;
        }
    }
}

// Συνάρτηση: ερωτήματος Α ( Υπολογισμός για πόσες τιμές έχουν μεγαλύτερη και πόσες μικρότερη από τη μέση τιμή )
void greaterLess(int countGreater, int countLess, int sendCounts[], int my_rank, int localArrayX[], float avg, int *totalCountGreater, int root, int *totalCountLess)
{

    int i = 0;

    // Έλεγχος σχέσης των στοιχείων localArrayX με την τιμή της μέσης τιμής
    countGreater = 0;
    countLess = 0;
    for (i = 0; i < sendCounts[my_rank]; i++)
    {
        if (localArrayX[i] > avg)
            countGreater++;
        else if (localArrayX[i] < avg)
            countLess++;
        else
        {
            printf("localArray[%d] = %d is equal with avg = %.2f\n", i, localArrayX[i], avg);
        }
    }

    //printf("Hello I am proccess %d and countGreater = %d while countLess = %d\n", my_rank, countGreater, countLess);

    // Send the information to the root
    MPI_Reduce(&countGreater, totalCountGreater, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
    MPI_Reduce(&countLess, totalCountLess, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
}

// Συνάρτηση: ερώτημα Β ( Υπολογισμός Διασποράς )
void var(float numerator, int sendCounts[], int my_rank, int localArrayX[], float avg, float *totalNumerators, int root, int arrayX_size)
{
    int i;
    numerator = 0.0;

    for (i = 0; i < sendCounts[my_rank]; i++)
    {
        numerator += (localArrayX[i] - avg) * (localArrayX[i] - avg);
    }

    // Reduce & MPI_SUM  numerator to the root and store it to totalNumerators
    MPI_Reduce(&numerator, totalNumerators, 1, MPI_FLOAT, MPI_SUM, root, MPI_COMM_WORLD);
}

void freeSpace(double arrayD[], int arrayX[], double localArrayD[], int localArrayX[], int sendCounts[], int displacements[], int my_rank)
{
    if (my_rank == 0)
    {
        free(arrayD);
        free(arrayX);
    }

    free(localArrayD);
    free(localArrayX);
    free(sendCounts);
    free(displacements);
}