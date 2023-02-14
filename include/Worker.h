#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <include/Mail.h>

using namespace std;

enum TaskType
{
	PARSE_EMAIL
};

class Worker
{
private:
	thread workerThread;
	TaskType taskType;
	vector<string> emailFiles;

public:
	Worker(TaskType taskType, vector<string> emailFiles);
	Worker(const Worker &worker) = delete;
	Worker &operator=(const Worker &worker) = delete;
	~Worker();

	void start();
};