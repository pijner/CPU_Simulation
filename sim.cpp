//***************************************************************************************
// Author : Prahar Ijner
// CSCI 375 - Operating Systems
// Programming assignment 1
// @params: <algorithm> [time quantum if algorithm == RR] <File name for Jobs>
//
// This program simulates FCFS, SJF, and RR scheduling on a virtual single processor CPU
// Required files to run: CPU.h, stats.h, PCB.h
//***************************************************************************************


#include <fstream>
#include <sstream>
#include "CPU.h"

using namespace std;

void FCFS(queue<string>);           // Function to perform first come first serve scheduling
void SJF(queue<string>);            // Function to perform shortest job first scheduling
void RR(queue<string>, int );       // Function to perform round robin scheduling

int main(int argc, char *argv[])
{

    string algorithm, filename;
    int q = 0;
    // If the user enters wrong command line parameters
    if(argc > 4 || argc < 3){
        cout << "Invalid parameters\n";
        cout << "Usage: <algorithm> <JobQueue file name>\n";
        return -1;
    }

    // Round robin scheduling requires an extra parameter (time quantum)
    else if(argc == 4){
        algorithm = string(argv[1]);
        filename = string(argv[3]);
        q = atoi(argv[2]);
    }
    // FCFS or SJF
    else{
        algorithm = string(argv[1]);
        filename = string(argv[2]);
    }

    // Queue to hold all the jobs
    queue<string> JobQueue;

    ifstream JobQueueFile(filename);
    string currJob;

    // Read the jobs file and populate the JobQueue
    while(getline(JobQueueFile, currJob))
        JobQueue.push(currJob);

    // Call functions based on algorithm selected
    if(algorithm == "FCFS")
        FCFS(JobQueue);
    else if(algorithm == "SJF")
        SJF(JobQueue);
    else if(algorithm == "RR")
        RR(JobQueue, q);        // Note : the RR function will take care of invalid time quantum
    else
        cout << "Invalid algorithm name. Available options: FCFS, SJF, RR";

    return 0;
}

/**
 * FCFS - Simulates a First Come First Serve algorithm in a virtual single processor CPU
 * @param: JobQ - contains a queue of strings describing a job
           Format of each job in JobQ:
                <ID> <ArrivalTime> <NumberOfBursts> <n numbers denoting length of each burst>
 * Prints CPU stats every 200 units of CPU time and summary stats at the end of simulation
*/
void FCFS(queue<string> JobQ){
    CPU myCPU;                          // Create virtual CPU (check CPU.h for implementation)
    stats myStats;                      // Create an object to hold the run time statistics

    // We need to run the CPU as long as there are pending jobs in the queue or memory
    while(myCPU.memoryUsed() || JobQ.size()){

        // Insert jobs into the ready queue if memory permits
        while(myCPU.insertToReady(&JobQ));

        // If all jobs in memory are waiting for IO
        if(!myCPU.readyJobs()){
            myCPU.incCPUTime();
            continue;
        }

        PCB currentJob = myCPU.getCurrentJob();
        // Is this is first burst of CPU the job is receiving?
        if(currentJob.getCurrentBurstNum() == 0){
            myCPU.startJob();
            myCPU.serviceJob();
        }

        // If this burst cycle is complete
        else if(currentJob.getCurrentBurstPC() == currentJob.getThisBurst()){

            // Was this the last burst cycle? If yes, the process is complete
            if(currentJob.getCurrentBurstNum() == currentJob.getNumBursts())
                myStats.addStats(myCPU.completeJob());  // Complete job and record stats for summary at end of simulation

            // The process is blocking for IO
            else{
                myCPU.insertToIOBlock(currentJob);
            }
            continue;
        }
        // This burst cycle isn't complete, so continue processing it
        else
            myCPU.serviceJob();

        // Increment CPU clock by 1 and check if any jobs finished IO
        myCPU.incCPUTime();

        // Print state of CPU every 200 units of time
        if(myCPU.getCPUTime()%200 == 0 && myCPU.getCPUTime())
            myCPU.printCPUState();
    }

    // Print summary statistics of simulation
    cout << "FCFS\n";
    cout << "CPU clock value = " << myCPU.getCPUTime() << endl;
    myStats.printStats();
}

/**
 * SJF - Simulates a Shortest Job First algorithm in a virtual single processor CPU
 * @param: JobQ - contains a queue of strings describing a job
           Format of each job in JobQ:
                <ID> <ArrivalTime> <NumberOfBursts> <n numbers denoting length of each burst>
 * Prints CPU stats every 200 units of CPU time and summary stats at the end of simulation
*/
void SJF(queue<string> JobQ){
    CPU myCPU;                          // Create virtual CPU (check CPU.h for implementation)
    stats myStats;                      // Create an object to hold the run time statistics

    // We need to run the CPU as long as there are pending jobs in the queue or memory
    while(myCPU.memoryUsed() || JobQ.size()){
        // Insert jobs into the ready queue if memory permits
        while(myCPU.insertToReady(&JobQ));

        // If all jobs in memory are waiting for IO
        if(!myCPU.readyJobs()){
            myCPU.incCPUTime();
            continue;
        }

        // If a job has finished running, find the next shortest job and execute it
        if(myCPU.getCurrentJob().getState() == STATE_READY)
            myCPU.sortReadyQ();

        PCB currentJob = myCPU.getCurrentJob();
        // Is this is first burst of CPU the job is receiving?
        if(currentJob.getCurrentBurstNum() == 0){
            myCPU.startJob();
            myCPU.serviceJob();
        }

        // If this burst cycle is complete
        else if(currentJob.getCurrentBurstPC() == currentJob.getThisBurst()){

            // Was this the last burst cycle? If yes, the process is complete
            if(currentJob.getCurrentBurstNum() == currentJob.getNumBursts())
                myStats.addStats(myCPU.completeJob()); // Complete job and record stats for summary at end of simulation

            // The process is blocking for IO
            else
                myCPU.insertToIOBlock(currentJob);

            continue;
        }
        // This burst cycle isn't complete, so continue processing it
        else
            myCPU.serviceJob();

        myCPU.incCPUTime();
    }

    cout << "SJF:\n";
    cout << "CPU clock value = " << myCPU.getCPUTime() << endl;
    myStats.printStats();
}

/**
 * RR - Simulates a Round robin preemption on FCFS algorithm in a virtual single processor CPU
 * @param: JobQ - contains a queue of strings describing a job
           Format of each job in JobQ:
                <ID> <ArrivalTime> <NumberOfBursts> <n numbers denoting length of each burst>

           quantum - positive integer value specifying the time quantum for round robin preemption
 * Prints CPU stats every 200 units of CPU time and summary stats at the end of simulation
*/
void RR(queue<string> JobQ, int quantum){
    CPU myCPU;                          // Create virtual CPU (check CPU.h for implementation)
    stats myStats;                      // Create an object to hold the run time statistics


    if(!myCPU.writeQuantum(quantum)){   // Time quantum must be a positive integer
        cout << "Invalid time quantum!";
        return;
    }

    // We need to run the CPU as long as there are pending jobs in the queue or memory
    while(myCPU.memoryUsed() || JobQ.size()){
        // Insert jobs into the ready queue if memory permits
        while(myCPU.insertToReady(&JobQ));

        // If all jobs in memory are waiting for IO
        if(!myCPU.readyJobs()){
            myCPU.incCPUTime();
            continue;
        }

        PCB currentJob = myCPU.getCurrentJob();
        // Is this is first burst of CPU the job is receiving?
        if(currentJob.getCurrentBurstNum() == 0){
            myCPU.startJob();
            myCPU.serviceJob();
        }
        else if(currentJob.getCurrentBurstPC() == currentJob.getThisBurst()){

            // Was this the last burst cycle? If yes, the process is complete
            if(currentJob.getCurrentBurstNum() == currentJob.getNumBursts())
                myStats.addStats(myCPU.completeJob());      // Complete job and record stats for summary at end of simulation

            // The process is blocking for IO
            else{
                myCPU.insertToIOBlock(currentJob);
            }
            continue;
        }

        // If the time quantum expired, preempt this job, add it to the end of the ready queue
        else if(currentJob.getCurrentQuantumTime() == myCPU.getQuantum()){
            myCPU.preemptJob();
            continue;
        }
        else
            myCPU.serviceJob();

        // Increment CPU clock by 1 and check if any jobs finished IO
        myCPU.incCPUTime();
    }

    cout << "RR\n";
    cout << "CPU clock value = " << myCPU.getCPUTime() << endl;
    myStats.printStats();
}


