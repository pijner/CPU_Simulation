/**
 * Author : Prahar Ijner
 * This file contains the stats class used to store the summary statistics during CPU scheduling simulation
 *
 */

#ifndef STATS_H_INCLUDED
#define STATS_H_INCLUDED

#include<iostream>

using namespace std;

class stats{
private:
    int pTime, wTime, taTime, shots, n;
public:
    stats();
    stats(int processingTime, int waitTime, int turnAroundTime, int shotsUsed);
    int getPTime()  { return pTime; }           // Read the processing time
    int getwTime()  { return wTime; }           // Read the waiting time
    int gettaTime() { return taTime; }          // Read the turn around time
    int getShots()  { return shots; }           // Read the number of shots used
    void addStats(stats);
    void printStats();
};

// Default constructor
stats::stats(){
    pTime = 0;
    wTime = 0;
    taTime = 0;
    shots = 0;
    n = 0;
}

// Overloaded constructor to initialize based on input values
stats::stats(int processingTime, int waitTime, int turnAroundTime, int shotsUsed){
    pTime = processingTime;
    wTime = waitTime;
    taTime = turnAroundTime;
    shots = shotsUsed;
    n = 1;
}

// Function to add the processing time, waiting time, turn around time, and number of shots used of this object and another
void stats::addStats(stats s){
    pTime += s.getPTime();
    wTime += s.getwTime();
    taTime += s.gettaTime();
    shots += s.getShots();
    n++;
}

// Function to print the average of all statistics collected (used at the end of simulation)
void stats::printStats(){
    cout << "Average processing time = " << ((float)pTime)/n << endl;
    cout << "Average waiting time = " << ((float)wTime)/n << endl;
    cout << "Average turn around time = " << ((float)taTime)/n << endl;
    cout << "Average shots used = " << ((float)shots)/n << endl;
}


#endif // STATS_H_INCLUDED
