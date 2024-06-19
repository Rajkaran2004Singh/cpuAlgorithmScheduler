#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include<bits/stdc++.h>
using namespace std;

vector<int>finishTime;
vector<int>turnAroundTime;
string operation;
int last, no_of_process;
vector<tuple<string,int,int>> processes;
vector<vector<char>>timeline;
vector<pair<char, int>> algorithms;
unordered_map<string,int>processToIndex;

void parse_algo(string algo_name)
{
    
    stringstream stream(algo_name);
    
    while (stream.good())
    {
        string temp_str;
        getline(stream, temp_str, ',');
        stringstream ss(temp_str);
        getline(ss, temp_str, '-');
        char algorithm_id = temp_str[0];
        getline(ss, temp_str, '-');
        int quantum = temp_str.size() >= 1 ? stoi(temp_str) : -1;
        algorithms.push_back( make_pair(algorithm_id, quantum) );
    }

}

void parse_process(){

    string process_name,process_info;
    int process_arrival_time, process_burst_time;

    for(int i=0;i<no_of_process;i++){

        cin >> process_info;

        stringstream stream(process_info);

        string temp_str;
        getline(stream, temp_str, ',');
        process_name = temp_str;
        getline(stream, temp_str, ',');
        process_arrival_time = stoi(temp_str);
        getline(stream, temp_str, ',');
        process_burst_time = stoi(temp_str);

        processes.push_back( make_tuple(process_name, process_arrival_time, process_burst_time) );
        processToIndex[process_name] = i; 

    }

}

void parse(){

    string algo_name;
    
    cin >> operation >> algo_name >> last >> no_of_process;

    parse_algo(algo_name);
    parse_process();

    finishTime.resize(no_of_process);
    timeline.resize(last);
    turnAroundTime.resize(no_of_process);

    for(int i=0; i<last; i++){
        for(int j=0; j<no_of_process; j++){
            timeline[i].push_back(' ');
        }
    }    

}

#endif