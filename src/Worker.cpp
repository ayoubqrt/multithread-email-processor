#include <include/Worker.h>

Worker::Worker(TaskType taskType, vector<string> emailFiles)
{
	this->taskType = taskType;
	this->emailFiles = emailFiles;

	if (taskType == PARSE_EMAIL)
	{
		thread myThread = thread(parseEmail, emailFiles);
		this->workerThread = move(myThread);
	}
	// If others task types are added, add a new if statement here
}

Worker::~Worker()
{
}

void Worker::start()
{
	this->workerThread.join();
}
