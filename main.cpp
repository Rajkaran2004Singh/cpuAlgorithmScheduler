#include <bits/stdc++.h>
#include "parser.h"

using namespace std;

const string TRACE = "trace";
const string SHOW_DETAILS = "details";
const string ALGORITHMS[9] = {"", "FCFS", "RR-", "SPN", "SRT", "HRRN", "FB-1", "FB-2i", "AGING"};

bool sortByBurstTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return (get<2>(a) < get<2>(b));
}
bool sortByArrivalTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return (get<1>(a) < get<1>(b));
}

bool descendingly_by_response_ratio(tuple<string, double, int> a, tuple<string, double, int> b)
{
    return get<1>(a) > get<1>(b);
}

bool byPriorityLevel(const tuple<int, int, int> &a, const tuple<int, int, int> &b)
{
    if (get<0>(a) == get<0>(b))
        return get<2>(a) > get<2>(b);
    return get<0>(a) > get<0>(b);
}

string getProcessName(tuple<string, int, int> &a)
{
    return get<0>(a);
}

int getArrivalTime(tuple<string, int, int> &a)
{
    return get<1>(a);
}

int getBurstTime(tuple<string, int, int> &a)
{
    return get<2>(a);
}

int getPriorityLevel(tuple<string, int, int> &a)
{
    return get<2>(a);
}

void clear_timeline()
{
    for (int i = 0; i < last; i++)
        for (int j = 0; j < no_of_process; j++)
            timeline[i][j] = ' ';
}

void firstComeFirstServe()
{

    sort(processes.begin(), processes.end(), sortByArrivalTime);

    int time = getArrivalTime(processes[0]);
    for (int i = 0; i < no_of_process; i++)
    {
        int processIndex = i;
        int arrivalTime = getArrivalTime(processes[i]);
        int burstTime = getBurstTime(processes[i]);

        finishTime[processIndex] = time + burstTime;
        turnAroundTime[processIndex] = finishTime[processIndex] - arrivalTime;

        for (int j = time; j < finishTime[processIndex]; j++)
            timeline[j][processIndex] = '*';
        for (int j = arrivalTime; j < time; j++)
            timeline[j][processIndex] = '.';
        time += burstTime;
    }
}

void roundRobin(int quantum)
{
    queue<pair<int, int>> q;
    int j = 0;
    if (getArrivalTime(processes[j]) == 0)
    {
        q.push(make_pair(j, getBurstTime(processes[j])));
        j++;
    }
    int currentQuantum = quantum;
    for (int time = 0; time < last; time++)
    {
        if (!q.empty())
        {
            int processIndex = q.front().first;
            q.front().second = q.front().second - 1;
            int remainingServiceTime = q.front().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getBurstTime(processes[processIndex]);
            currentQuantum--;
            timeline[time][processIndex] = '*';
            while (j < no_of_process && getArrivalTime(processes[j]) == time + 1)
            {
                q.push(make_pair(j, getBurstTime(processes[j])));
                j++;
            }

            if (currentQuantum == 0 && remainingServiceTime == 0)
            {
                finishTime[processIndex] = time + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                currentQuantum = quantum;
                q.pop();
            }
            else if (currentQuantum == 0 && remainingServiceTime != 0)
            {
                q.pop();
                q.push(make_pair(processIndex, remainingServiceTime));
                currentQuantum = quantum;
            }
            else if (currentQuantum != 0 && remainingServiceTime == 0)
            {
                finishTime[processIndex] = time + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                q.pop();
                currentQuantum = quantum;
            }
        }
        while (j < no_of_process && getArrivalTime(processes[j]) == time + 1)
        {
            q.push(make_pair(j, getBurstTime(processes[j])));
            j++;
        }
    }
    fillInWaitTime();
}

void shortestProcessNext()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    int j = 0;
    for (int i = 0; i < last; i++)
    {
        while (j < no_of_process && getArrivalTime(processes[j]) <= i)
        {
            pq.push(make_pair(getBurstTime(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int processIndex = pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int burstTime = getBurstTime(processes[processIndex]);
            pq.pop();

            int temp = arrivalTime;
            for (; temp < i; temp++)
                timeline[temp][processIndex] = '.';

            temp = i;
            for (; temp < i + burstTime; temp++)
                timeline[temp][processIndex] = '*';

            finishTime[processIndex] = (i + burstTime);
            turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
            i = temp - 1;
        }
    }
}

void shortestRemainingTime()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    int j = 0;
    for (int i = 0; i < last; i++)
    {
        while (j < no_of_process && getArrivalTime(processes[j]) == i)
        {
            pq.push(make_pair(getBurstTime(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int processIndex = pq.top().second;
            int remainingTime = pq.top().first;
            pq.pop();
            int serviceTime = getBurstTime(processes[processIndex]);
            int arrivalTime = getArrivalTime(processes[processIndex]);
            timeline[i][processIndex] = '*';

            if (remainingTime == 1)
            {
                finishTime[processIndex] = i + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
            }
            else
            {
                pq.push(make_pair(remainingTime - 1, processIndex));
            }
        }
    }
    fillInWaitTime();
}

void highestResponseRatioNext()
{
    vector<tuple<string, double, int>> present_processes;
    int j = 0;
    for (int current_instant = 0; current_instant < last; current_instant++)
    {
        while (j < no_of_process && getArrivalTime(processes[j]) <= current_instant)
        {
            present_processes.push_back(make_tuple(getProcessName(processes[j]), 1.0, 0));
            j++;
        }

        for (auto &process : present_processes)
        {
            string process_name = get<0>(process);
            int process_index = processToIndex[process_name];
            int wait_time = current_instant - getArrivalTime(processes[process_index]);
            int service_time = getBurstTime(processes[process_index]);
            get<1>(process) = calculate_response_ratio(wait_time, service_time);
        }

        sort(present_processes.begin(), present_processes.end(), descendingly_by_response_ratio);

        if (!present_processes.empty())
        {
            int process_index = processToIndex[get<0>(present_processes[0])];
            while (current_instant < last && get<2>(present_processes[0]) != getBurstTime(processes[process_index]))
            {
                timeline[current_instant][process_index] = '*';
                current_instant++;
                get<2>(present_processes[0])++;
            }
            current_instant--;
            present_processes.erase(present_processes.begin());
            finishTime[process_index] = current_instant + 1;
            turnAroundTime[process_index] = finishTime[process_index] - getArrivalTime(processes[process_index]);
        }
    }
    fillInWaitTime();
}

void feedbackQ1()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    unordered_map<int, int> remainingServiceTime;
    int j = 0;
    if (getArrivalTime(processes[0]) == 0)
    {
        pq.push(make_pair(0, j));
        remainingServiceTime[j] = getBurstTime(processes[j]);
        j++;
    }
    for (int time = 0; time < last; time++)
    {
        if (!pq.empty())
        {
            int priorityLevel = pq.top().first;
            int processIndex = pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getBurstTime(processes[processIndex]);
            pq.pop();
            while (j < no_of_process && getArrivalTime(processes[j]) == time + 1)
            {
                pq.push(make_pair(0, j));
                remainingServiceTime[j] = getBurstTime(processes[j]);
                j++;
            }
            remainingServiceTime[processIndex]--;
            timeline[time][processIndex] = '*';
            if (remainingServiceTime[processIndex] == 0)
            {
                finishTime[processIndex] = time + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
            }
            else
            {
                if (pq.size() >= 1)
                    pq.push(make_pair(priorityLevel + 1, processIndex));
                else
                    pq.push(make_pair(priorityLevel, processIndex));
            }
        }
        while (j < no_of_process && getArrivalTime(processes[j]) == time + 1)
        {
            pq.push(make_pair(0, j));
            remainingServiceTime[j] = getBurstTime(processes[j]);
            j++;
        }
    }
    fillInWaitTime();
}

void feedbackQ2i()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    unordered_map<int, int> remainingServiceTime;
    int j = 0;
    if (getArrivalTime(processes[0]) == 0)
    {
        pq.push(make_pair(0, j));
        remainingServiceTime[j] = getBurstTime(processes[j]);
        j++;
    }
    for (int time = 0; time < last; time++)
    {
        if (!pq.empty())
        {
            int priorityLevel = pq.top().first;
            int processIndex = pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getBurstTime(processes[processIndex]);
            pq.pop();
            while (j < no_of_process && getArrivalTime(processes[j]) <= time + 1)
            {
                pq.push(make_pair(0, j));
                remainingServiceTime[j] = getBurstTime(processes[j]);
                j++;
            }

            int currentQuantum = pow(2, priorityLevel);
            int temp = time;
            while (currentQuantum && remainingServiceTime[processIndex])
            {
                currentQuantum--;
                remainingServiceTime[processIndex]--;
                timeline[temp++][processIndex] = '*';
            }

            if (remainingServiceTime[processIndex] == 0)
            {
                finishTime[processIndex] = temp;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
            }
            else
            {
                if (pq.size() >= 1)
                    pq.push(make_pair(priorityLevel + 1, processIndex));
                else
                    pq.push(make_pair(priorityLevel, processIndex));
            }
            time = temp - 1;
        }
        while (j < no_of_process && getArrivalTime(processes[j]) <= time + 1)
        {
            pq.push(make_pair(0, j));
            remainingServiceTime[j] = getBurstTime(processes[j]);
            j++;
        }
    }
    fillInWaitTime();
}

void aging(int quantum)
{
    vector<tuple<int, int, int>> v;
    int j = 0, currentProcess = -1;
    for (int time = 0; time < last; time++)
    {
        while (j < no_of_process && getArrivalTime(processes[j]) <= time)
        {
            v.push_back(make_tuple(getPriorityLevel(processes[j]), j, 0));
            j++;
        }

        for (int i = 0; i < v.size(); i++)
        {
            if (get<1>(v[i]) == currentProcess)
            {
                get<2>(v[i]) = 0;
                get<0>(v[i]) = getPriorityLevel(processes[currentProcess]);
            }
            else
            {
                get<0>(v[i])++;
                get<2>(v[i])++;
            }
        }
        sort(v.begin(), v.end(), byPriorityLevel);
        currentProcess = get<1>(v[0]);
        int currentQuantum = quantum;
        while (currentQuantum-- && time < last)
        {
            timeline[time][currentProcess] = '*';
            time++;
        }
        time--;
    }
    fillInWaitTime();
}

void execute_algo(char algo_id, int quantum, string operation)
{

    switch (algo_id)
    {
    case '1':
        if (operation == TRACE)
            cout << "FCFS" << " ";
        firstComeFirstServe();
        break;
    case '2':
        if (operation == TRACE)
            cout << "RR-" << quantum << "  ";
        roundRobin(quantum);
        break;
    case '3':
        if (operation == TRACE)
            cout << "SPN   ";
        shortestProcessNext();
        break;
    case '4':
        if (operation == TRACE)
            cout << "SRT   ";
        shortestRemainingTime();
        break;
    case '5':
        if (operation == TRACE)
            cout << "HRRN  ";
        highestResponseRatioNext();
        break;
    case '6':
        if (operation == TRACE)
            cout << "FB-1  ";
        feedbackQ1();
        break;
    case '7':
        if (operation == TRACE)
            cout << "FB-2i ";
        feedbackQ2i();
        break;
    case '8':
        if (operation == TRACE)
            cout << "Aging ";
        aging(quantum);
        break;
    default:
        break;
    }
}

double calculate_response_ratio(int wait_time, int burst_time)
{
    return (wait_time + burst_time) * 1.0 / burst_time;
}

void fillInWaitTime()
{
    for (int i = 0; i < no_of_process; i++)
    {
        int arrivalTime = getArrivalTime(processes[i]);
        for (int k = arrivalTime; k < finishTime[i]; k++)
        {
            if (timeline[k][i] != '*')
                timeline[k][i] = '.';
        }
    }
}

void printTimeline(int algorithm_index)
{
    for (int i = 0; i <= last; i++)
        cout << i % 10 << " ";
    cout << "\n";
    cout << "--------------------------------------------------------------------\n";
    for (int i = 0; i < no_of_process; i++)
    {
        cout << getProcessName(processes[i]) << "     |";
        for (int j = 0; j < last; j++)
        {
            cout << timeline[j][i] << "|";
        }
        cout << " \n";
    }
    cout << "--------------------------------------------------------------------\n";
}

void printAlgorithm(int algorithm_index)
{
    int algorithm_id = algorithms[algorithm_index].first - '0';
    if (algorithm_id == 2)
        cout << ALGORITHMS[algorithm_id] << algorithms[algorithm_index].second << endl;
    else
        cout << ALGORITHMS[algorithm_id] << endl;
}

void printProcesses()
{
    cout << "Process    ";
    for (int i = 0; i < no_of_process; i++)
        cout << "|  " << getProcessName(processes[i]) << "  ";
    cout << "|\n";
}

void printArrivalTime()
{
    cout << "Arrival    ";
    for (int i = 0; i < no_of_process; i++)
        printf("|%3d  ", getArrivalTime(processes[i]));
    cout << "|\n";
}
void printBurstTime()
{
    cout << "Service    |";
    for (int i = 0; i < no_of_process; i++)
        printf("%3d  |", getBurstTime(processes[i]));
    cout << " Mean|\n";
}
void printFinishTime()
{
    cout << "Finish     ";
    for (int i = 0; i < no_of_process; i++)
        printf("|%3d  ", finishTime[i]);
    cout << "|-----|\n";
}
void printTurnAroundTime()
{
    cout << "Turnaround |";
    int sum = 0;
    for (int i = 0; i < no_of_process; i++)
    {
        printf("%3d  |", turnAroundTime[i]);
        sum += turnAroundTime[i];
    }
    if ((1.0 * sum / turnAroundTime.size()) >= 10)
        printf("%2.2f|\n", (1.0 * sum / turnAroundTime.size()));
    else
        printf(" %2.2f|\n", (1.0 * sum / turnAroundTime.size()));
}

void printStats(int algorithm_index)
{
    printAlgorithm(algorithm_index);
    printProcesses();
    printArrivalTime();
    printBurstTime();
    printFinishTime();
    printTurnAroundTime();
}

int main()
{

    parse();

    for (int i = 0; i < (int)algorithms.size(); i++)
    {

        clear_timeline();

        execute_algo(algorithms[i].first, algorithms[i].second, operation);

        if (operation == TRACE)
        {
            printTimeline(i);
        }
        if (operation == SHOW_DETAILS)
        {
            printStats(i);
        }

    }
}