#include <iostream>
#include <thread>
#include <vector>

using namespace std;
enum TaskType
{
	PARSE_EMAIL
};

class Worker
{
private:
	vector<string> emailFiles;
	thread::id currentThreadId;
	thread workerThread;
	TaskType taskType;

public:
	Worker(TaskType taskType, vector<string> emailFiles);
	~Worker();

	void start();
};