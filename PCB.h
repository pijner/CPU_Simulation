/**
 * Author : Prahar Ijner
 * This file contains the PCB (Program Control Block) class, which holds the details of a program in memory.
 *
 * Requires stats.h
 */

#ifndef PCB_H_INCLUDED
#define PCB_H_INCLUDED

#include <iostream>
#include <vector>
#include "stats.h"

#define STATE_READY 0
#define STATE_IOB -1
#define STATE_RUN 1
#define IO_TIME 10

using namespace std;

class PCB{
private:
    int jobID, state, PC, nBursts, cBurst, IOCompTime, startTime, endTime, arrivalTime, cBurstPC, quantumTime, numShots;
    vector<int> burstList;
public:
    PCB(int jID, int aTime, int nB, vector<int> bL);
    int getID()                 { return jobID; }                       // Get the ID of this job
    void setIOCompTime(int t)   { IOCompTime = t; }                     // Set the time at which IO operation will complete
    int  getIOCompTime()        { return IOCompTime; }                  // Get the time at which IO operation will complete
    void writeEndTime(int t)    { endTime = t; }                        // Write the time at which the process finished execution
    int  getCurrentBurstNum()   { return cBurst; }                      // Get which burst this process is on
    void setState(int s)        { state = s; }                          // Change running state of the process (look at defined constants for states)
    int  getState()             { return state; }                       // Get the running state of the process
    void setStartTime(int t)    { startTime = t; }                      // Write when this process received the first CPU cycle
    int  getCurrentBurstPC()    { return cBurstPC; }                    // Get number of cycles current burst has received
    void incCurrentBurstPC()    { cBurstPC++; PC++; quantumTime++; }    // Give this process 1 clock cycle (increases PC and number of cycles used in current quantum)
    int  getThisBurst()         { return burstList.at(cBurst-1); }      // Get number of cycles current burst needs
    int  getNumBursts()         { return nBursts; }                     // Get number of bursts this process has
    void resetQuantum()         { quantumTime = 0; }                    // Reset number of cycles used in present quantum (used at the end of quantum or end of burst)
    void shotComplete()         { numShots++; }                         // Increase the number of shots used by 1
    void printStats();
    void burstComplete();
    int  getCurrentQuantumTime(){ return quantumTime; }
    stats getStats();
};

// Class constructor to initialize PCB using details from JobQueue
PCB::PCB(int jID, int aTime, int nB, vector<int> bL){
    jobID = jID;
    arrivalTime = aTime;
    nBursts = nB;
    burstList = bL;

    // Initialize other data members to standard value
    state = STATE_READY;
    PC = 0;
    cBurst = 1;
    IOCompTime = 0;
    cBurstPC= 0;
    quantumTime = 0;
    numShots = 0;
}

// Function to return stats of the current process (used at the end of every process)
stats PCB::getStats(){
    int processingTime = PC+IO_TIME*(nBursts-1);        // Processing time = time spent on CPU + 10*Number of IO operations
    int tat = endTime - arrivalTime;                    // Turn around time = end time - arrival time
    int waitTime = tat - processingTime;                // waiting time = turn around time - processing time
    stats s(processingTime, waitTime, tat, numShots );

    return s;
}

// Function to print the stats of the process (used at the end of every process)
void PCB::printStats(){
    cout << "ID: " << jobID << endl;
    cout << "Arrived at " << arrivalTime << endl;
    cout << "Completed at " << endTime << endl;
    cout << "Processing time " << PC+IO_TIME*(nBursts-1) << endl;
    int tat = endTime - arrivalTime;
    cout << "Waiting time " << tat - (PC+IO_TIME*(nBursts-1)) << endl;
    cout << "Turn around time " << tat << endl;
    cout << "CPU shots used " << numShots << endl << endl;
}

// Function to signal the current burst has been completed
void PCB::burstComplete(){
    cBurst++;               // Increment current burst value to next burst
    numShots++;             // Increment number of CPU shots used
    state = STATE_IOB;      // Change state to blocking for IO
    cBurstPC = 0;           // Reset number of cycles consumed in current burst
    quantumTime = 0;        // Reset number of cycles used in current quantum burst
}

// Comparison function to sort the processes based on the time for current burst. (Used for SJF)
bool isBurstGreaterThan(PCB a, PCB b){
    return (a.getThisBurst() < b.getThisBurst()) ;
}
#endif // PCB_H_INCLUDED
