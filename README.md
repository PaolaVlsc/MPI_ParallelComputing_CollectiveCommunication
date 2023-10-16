# MPI_ParallelComputing_CollectiveCommunication
A uni project on the subject "Introduction to Parallel Computing". Parallel Computing using MPI Collective Communication in C. 

# MPI Collective Communication Example

This repository provides a practical example of MPI (Message Passing Interface) collective communication in C. Collective communication operations involve groups of processes working together to exchange data or synchronize their activities. In this example, we explore how to use various collective communication routines in an MPI program to enhance parallel computing capabilities.

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Usage](#usage)
- [Collective Communication Operations](#collective-communication-operations)
- [Logic](#logic-and-explanation-of-code)
- [Contributing](#contributing)

## Introduction

Collective communication is a fundamental concept in parallel and distributed computing, allowing processes to work collectively and efficiently solve complex problems. In this repository, you will find practical examples of MPI collective communication operations, including broadcast, scatter, gather, and reduce.

## Prerequisites

Before running this example, ensure you have the following prerequisites:

1. Make sure that mpi is installed on your system.

2. A C compiler (e.g., `mpicc`) for building the MPI programs.

## Usage

1. **Clone this repository** to your local machine:

    ```shell
    git clone https://github.com/your-username/MPI_Collective_Communication.git
    ```

2. **Navigate to the project directory**:

    ```shell
    cd MPI_Collective_Communication
    ```

3. **Compile the source code** using an MPI compiler. For example, if you're using `mpicc`:

    ```shell
    mpicc mpi_collective_example.c -o mpi_collective_example
    ```

4. **Run the example** using the `mpiexec` command. Be sure to specify the number of processes you want to use, as needed for the specific collective communication operation:

    ```shell
    mpiexec -np <num_processes> ./mpi_collective_example
    ```

## Collective Communication Operations

This example covers several important collective communication operations:

- **Broadcast:** Demonstrates the distribution of data from one process to all other processes.
- **Scatter:** Illustrates the division of data from one process among all other processes.
- **Gather:** Shows the collection of data from all processes into one process.
- **Reduce:** Displays the combination of data from all processes into a single result.

## Logic and explanation of code 

Our program initially starts with a main menu, which serves as the user interface. It prompts the user to choose from the following options:

* Input new data.
* Display student information.
* Terminate the program.

<p align="center">
    <img width="50%" src="https://github.com/PaolaVlsc/MPI_ParallelComputing_CollectiveCommunication/assets/87998374/18075a09-8e58-492b-8888-078431a2dec1" alt="results">
</p>

If the user selects option 1, the main program begins. Initially, it requests the user to input the length of array X. Afterward, it initializes this array with integer values provided by the user

Next, the program calculates how many numbers each process will receive and stores these counts in the array sendCounts[]. Additionally, it computes and initializes the displacements[] array (which indicates the offset of the block of elements that will be sent to each process from the starting point).

After initializing the local arrays `localArrayX`, the parallel computation of the mean value and the calculation of the min and max values follow. 

For calculating the mean value, each process sums the elements of its `localArrayX` and sends this sum back to the root process using the `MPI_Reduce` function with `MPI_SUM`. The final result is stored in the root's variable `tot_sum`, and then the root divides this value by the size of array `X`. 

For computing the minimum value, a similar procedure is followed. Each process finds the minimum value among the elements of its local array and sends it to the root process using `MPI_Reduce`. In combination with `MPI_MIN`, the process of finding the minimum value among the elements sent by processes is automated.

The same approach is applied to find the maximum value as well.

### Question no.1 : The number of elements in vector X that have values less than the mean value (m) and the number of elements greater than the mean value can be calculated as follows:

1. Count the number of elements xᵢ in X such that xᵢ < m.
2. Count the number of elements xᵢ in X such that xᵢ > m.

The first count represents the elements with values less than the mean, and the second count represents the elements with values greater than the mean.

Having already received the mean value from the root, each process independently calculates how many values in their local data are greater (countGreater) and how many are less (countLess) than the mean value. They then send these results back to the root using MPI_Reduce, where the sums are automatically computed.

The final results are stored in the root's variables, totalCountGreater and totalCountLess

<p align="center">
    <img width="50%" src="https://github.com/PaolaVlsc/MPI_ParallelComputing_CollectiveCommunication/assets/87998374/959eede0-4c99-4f18-b15b-071ac0274ee0" alt="results">
</p>


### Question no.2:  The variance of the elements in vector X.


Each process calculates the sum of the square of the difference ((xi - m)^2) between the elements in their local array localArrayX. After performing these calculations, they return the result to the root process using MPI_Reduce in combination with MPI_SUM.

Subsequently, the root, having already received the total numerators in the variable totalNumerators, divides it by arrayX_size and prints the final result of the variance.

<p align="center">
    <img width="50%" src="https://github.com/PaolaVlsc/MPI_ParallelComputing_CollectiveCommunication/assets/87998374/b6a54e3c-2159-44bb-8262-4c984fe86b1d" alt="results">
</p>

### Question no.3: A new vector Δ, where each element δᵢ is equal to the percentage relationship of the corresponding element (xᵢ) in vector X with the difference between the maximum and minimum values of all elements in vector X.

Having already received the information about min and max from the root, each process locally creates its own `localArrayD`, where the calculations δᵢ = ((xᵢ - x_min) / (x_max - x_min)) * 100 are stored for each element of their `localArrayX`.

After performing these calculations, each process sends their results to the root process using `MPI_Gather`.

<p align="center">
    <img width="50%" src="https://github.com/PaolaVlsc/MPI_ParallelComputing_CollectiveCommunication/assets/87998374/0197072c-d54c-4489-bd04-2c753d5bc23a" alt="results">
</p>

### Question no.4: The maximum value in vector Δ and the specific element xi that corresponds to it (including the position i of the element in the vector, the value of the element, and its δᵢ) is:

- Maximum value in Δ: δ_max
- Corresponding element xi: x_i
- Position i of x_i in the vector X
- Value of x_i: x_i
- δᵢ of x_i: δᵢ


By defining the `struct info` and the global variables `inmax`, `inmin`, `outmax`, and `outmin`, each process calculates the global position of its value and not its local index for the tasks at hand.

<p align="center">
    <img width="50%" src="https://github.com/PaolaVlsc/MPI_ParallelComputing_CollectiveCommunication/assets/87998374/0fb94151-a29c-4928-ba8c-38e8181fd7f3" alt="results">
</p>


### Author's note
* The program was implemented using iterative menu-driven functionality.
* The program operates correctly for cases where n is not a multiple of p (using Scatterv and Gatherv). However, it was observed that for n < p, calculations are not performed correctly.

## Contributing

If you have any improvements or suggestions for this example, please feel free to open an issue or create a pull request. Contributions are welcome!

This project was written by Velasco Paola.
