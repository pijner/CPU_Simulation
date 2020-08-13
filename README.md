# CPU_Simulation
This program simulates a few basic CPU scheduling algorithms


### Algorithms included
<ul>
  <li> First come first serve (FCFS) </li>
  <li> Shortest job first (SJF) </li>
  <li> Round robin (RR) </li>
</ul>


The memory can hold a maximum of 10 jobs at a time in the job queue. A job is loaded into memory by creating a Process Control Block (PCB) for the process and inserting it into the Ready queue. Once a job is completed, it is removed from the job queue and another job is loaded.

### Usage
    $ cpusim <algorithm> [time quantum if algorithm == RR] <File name for Jobs>
  The algorithm can be FCFS, SJF, or RR <br>
  If the algorithm is RR, the time quantum must be specified <br>
  The file name for jobs is the name of the file that includes a list of all jobs (in this case it is JobQueue.txt
