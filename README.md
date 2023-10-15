# MPI_ParallelComputing_CollectiveCommunication
A uni project on the subject "Introduction to Parallel Computing". Parallel Computing using MPI Collective Communication in C. 

# MPI Collective Communication Example

This repository provides a practical example of MPI (Message Passing Interface) collective communication in C. Collective communication operations involve groups of processes working together to exchange data or synchronize their activities. In this example, we explore how to use various collective communication routines in an MPI program to enhance parallel computing capabilities.

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Usage](#usage)
- [Collective Communication Operations](#collective-communication-operations)
- [Logic and explanation of code](#Logic-and-explanation-of-code)
- [Contributing](#contributing)
- [License](#license)

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

### Author's note


## Contributing

If you have any improvements or suggestions for this example, please feel free to open an issue or create a pull request. Contributions are welcome!

This project was written by Velasco Paola.
