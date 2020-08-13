/**
 * Author : Prahar Ijner
 * This file contains the CPU class. It is used to simulate a virtual single processor CPU.
 *
 * Requires PCB.h, stats.H
 */

#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "PCB.h"
#include <queue>
#include <algorithm>

#define MAX_JOBS 10

class CPU{
private:
    unsigned long CPUTime;
    int jobsInMemory, totalWait, quantum, jobsComplete;
    queue<PCB> readyQ, IOBlockeQ;
public:
    CPU(){
        // Default class constructor
        CPUTime = 0;
        jobsInMemory = 0;
        totalWait = 0;
        jobsComplete = 0;
    }
    unsigned long getCPUTime()  { return CPUTime; }         // Read the CPU time
    int  memoryUsed()           { return jobsInMemory; }    // Return the number of jobs in memory
    int  readyJobs()            { return readyQ.size(); }   // Return the number of jobs in ready queue
    int  getQuantum()           { return quantum; }         // Read the value of time quantum set for this CPU
    PCB  getCurrentJob()        { return readyQ.front(); }  // Get the job that is being or will begin executing
    bool writeQuantum(int q);
    void incCPUTime();
    bool insertToReady(queue<string> *jobQ);
    void insertToIOBlock(PCB jobToInsert);
    void serviceJob();
    void startJob();
    stats completeJob();
    bool sortReadyQ();
    void preemptJob();
    void printCPUState();

};

// Function to store value of time quantum for preemption of jobs
bool CPU::writeQuantum(int q){
    if(q<=0)            // Can't have non-positive time quantum
        return false;
    quantum = q;
    return true;
}

/* Function to add a job from job queue to the ready queue is possible
 * Returns false if job can't be added to the queue
 */
bool CPU::insertToReady(queue<string> *jobQ){
    // If memory is full or there are no more jobs in the job queue, return false
    if( jobsInMemory == MAX_JOBS || !jobQ->size() )
        return false;

    // Read the first line of the job queue and remove it from there as it is being added to the ready queue
    istringstream thisLine(jobQ->front());
    jobQ->pop();

    // Read job id, time of arrival into the system, and number of bursts
    int id, arrivalTime, numBursts;
    thisLine >> id >> arrivalTime >> numBursts;

    // Vector holding all the CPU bursts this process will need to complete execution
    vector<int> bursts;
    for(int i=0; i<numBursts; i++){
        int thisBurst;
        thisLine >> thisBurst;
        bursts.push_back(thisBurst);
    }

    // Create a PCB (defined in PCB.h) for the job just read from the job queue
    PCB job(id, arrivalTime, numBursts, bursts);
    // Add this new job to the ready queue and increment number of jobs in memory
    readyQ.push(job);
    jobsInMemory++;

    return true;
}

/* Function to increment CPU time by 1. It also checks if any job in the IO blocked queue has finished IO
 * and brings it back to the ready queue if it has
 */
void CPU::incCPUTime(){
    // If there's anything in the IO queue, did it just complete IO?
    if(IOBlockeQ.size()){
        PCB j = IOBlockeQ.front();
        if(j.getIOCompTime() == CPUTime){
            // If it completed IO, set it's state to ready and move it to the ready queue
            j.setState(STATE_READY);
            readyQ.push(j);
            IOBlockeQ.pop();
        }
    }
    // Increment CPU time
    CPUTime++;
}

/* Function to insert a job into the IO Blocked queue
 * It checks if there are any jobs in the IO Blocked queue and accordingly decides when this process will finish IO
 * NOTE : Only one job can perform IO at a given time
 */
void CPU::insertToIOBlock(PCB jobToInsert){
    // process enters into IO only after burst is complete. So signal that first
    jobToInsert.burstComplete();

    // If there are any jobs in the IO Queue, the job being queued has to wait for those to finish IO before it can perform IO
    if(IOBlockeQ.size()){
        PCB j = IOBlockeQ.front();
        // Total waiting time for IO = time for all jobs in IO queue to finish IO
        int IOwaitTime = (j.getIOCompTime() - CPUTime) + (IOBlockeQ.size() - 1)*IO_TIME;
        totalWait += IOwaitTime;
        jobToInsert.setIOCompTime(CPUTime + IOwaitTime + IO_TIME);
    }
    // If there are no jobs in IO queue, this job can right away perform IO
    else
        jobToInsert.setIOCompTime(CPUTime + IO_TIME);

    // Insert into the IO Blocked queue and remove from the ready queue
    jobToInsert.setState(STATE_IOB);
    IOBlockeQ.push(jobToInsert);
    readyQ.pop();
}

// Function to run the first job in the ready queue for 1 unit of time
void CPU::serviceJob(){
    PCB *currentJob = &(readyQ.front());
    currentJob->setState(STATE_RUN);
    currentJob->incCurrentBurstPC();
}

// Function to initialize start time of a job if it is being run for the first time
void CPU::startJob(){
    PCB *currentJob = &(readyQ.front());
    currentJob->setStartTime(CPUTime);
    currentJob->setState(STATE_RUN);
}

// Function to mark a job as complete, generate and return its completion statistics
stats CPU::completeJob(){
    PCB j = readyQ.front();
    j.shotComplete();
    j.writeEndTime(CPUTime);
    j.printStats();

    stats s = j.getStats();     // Generate the stats

    readyQ.pop();               // Remove this from the ready queue
    jobsInMemory--;             // Decrease number of jobs in memory by 1
    jobsComplete++;             // Increase the number of jobs completed

    return s;
}

// Function to sort the ready queue based on the shortest processing time of current burst (used in SJF)
bool CPU::sortReadyQ(){
    if (!readyQ.size())
        return false;

    // Empty the queue into a vector that can be sorted
    vector<PCB> tempVec;
    while(readyQ.size()){
        tempVec.push_back(readyQ.front());
        readyQ.pop();
    }
    // Sort the vector based on the processing time of burst (isBurstGreaterThan is defined in PCB.h)
    sort(tempVec.begin(), tempVec.end(), isBurstGreaterThan);
    // Add the sorted list back to the ready queue
    while(tempVec.size()){
        readyQ.push(tempVec.at(0));
        tempVec.erase(tempVec.begin());
    }
    return true;
}

// Function to preempt the first job in the ready queue (used when time quantum expires)
void CPU::preemptJob(){
    if(readyQ.size()){
        PCB *j = &(readyQ.front());
        j->setState(STATE_READY);       // Set state to ready
        j->resetQuantum();              // Reset the time quantum as it will start from 0 in the next quantum burst
        j->shotComplete();              // Signal the shot has been completed

        readyQ.push(readyQ.front());    // Add this to the end of the ready queue
        readyQ.pop();                   // Remove it from the front of the ready queue
    }
}


// Function to print the number of jobs in ready queue, IO blocked queue, and number of jobs completed
void CPU::printCPUState(){
    cout << "Number of jobs in ReadyQueue = " << readyJobs() << endl;
    cout << "Number of jobs in BlockedQueue = " << IOBlockeQ.size() << endl;
    cout << "Number of jobs completed = " << jobsComplete << endl << endl;
}

#endif // CPU_H_INCLUDED
