# cpuAlgorithmScheduler
This C++ project can be used for following CPU scheduling algorithms:
1.First come first serve(FCFS)
2.Round robin (RR)
3.Shortest Process Next(SPN)
4.Shortest Remaining Time(SRT)
5.Highest Response Ratio Next(HRRN)
6.Feedback(FB)
7.Feedback with varying time quantum
8.Aging

INPUT FORMAT
Line 1: trace or details

Line 2: a comma-separated list telling which CPU scheduling policies to be analyzed/visualized along with their parameters, if applicable. Each algorithm is represented by a number as listed in the introduction section. Round Robin and Aging have a parameter specifying the quantum q to be used. Therefore, a policy entered as 2-3 means Round Robin with q=3. Similarly, policy 8-7 means Aging with q=7.

Line 3: An integer specifying the last instant to be used in your simulation and to be shown on the timeline.

Line 4: Number of processes to be scheduled.

Line 5: Start of description of processes. Each process is to be described on a separate line. For algorithms 1 through 7, each process is described using a comma-separated list specifying:

1- String specifying a process name
2- Arrival Time
3- Service Time

Note: For Aging algorithm (algorithm 8), each process is described using a comma-separated list specifying:

1- String specifying a process name
2- Arrival Time
3- Priority

Processes are assumed to be sorted based on the arrival time. If two processes have the same arrival time, then the one with the lower priority is assumed to arrive first.

Sample Input for FCFS :
trace
1
20
5
A,0,3
B,2,6
C,4,4
D,6,5
E,8,2
