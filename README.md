# CPU_Simulation
This program simulates a few basic CPU scheduling algorithms


### Algorithms included
<ul>
  <li> First come first serve (FCFS) 
  <li> Shortest job first (SJF) 
  <li> Round robin (RR) 
</ul>


The memory can hold a maximum of 10 jobs at a time in the job queue. A job is loaded into memory by creating a Process Control Block (PCB) for the process and inserting it into the Ready queue. Once a job is completed, it is removed from the job queue and another job is loaded. It can be assumed that there's I/O activity between CPU bursts for each process. A process performing I/O is places in the blocked queue and is moved back into the ready queue once the I/O time (10 time units) is complete.

### Usage
    $ cpusim <algorithm> [time quantum if algorithm == RR] <File name for Jobs>
  The algorithm can be FCFS, SJF, or RR <br>
  If the algorithm is RR, the time quantum must be specified <br>
  The file name for jobs is the name of the file that includes a list of all jobs (in this case it is JobQueue.txt


### Input
  The input in this case is JobQueue.txt which consists of all the jobs to be simulated. A job described by the following <br>
  job_ID arrival_Time cpu_Bursts_Required cpu_Bursts_Sequence <br>
  
  
### Output
  #### After each job termination, the following is printed:
  <ul>
    <li> job ID
    <li> arrival time (time after start that job arrived to the system)
    <li> completion time (time after start that job was completed)
    <li> processing time (total time spent in CPU control + 10 units per I/O task)
    <li> waiting time
    <li> turnaround time
    <li> number of CPU bursts used by the job
  </ul>
  
  #### After every 200 time units, the following is printed:
  <ul>
    <li> number of jobs in ready queue
    <li> number of jobs in the blocked queue
    <li> number of jobs completed
  </ul>
  
  #### After completion of all jobs, the following is printed:
  <ul>
    <li> scheduling algorithm used
    <li> number of CPU clock cycles elapsed
    <li> average processing time
    <li> average waiting time
    <li> average turnaround time
    <li> average number of CPU bursts used by a job
  </ul>
