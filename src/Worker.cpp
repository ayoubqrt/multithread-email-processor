#include <include/Worker.h>
#include <include/Mail.h>
Worker::Worker(TaskType taskType, vector<string> emailFiles)
{
	this->taskType = taskType;
	this->emailFiles = emailFiles;
}

Worker::~Worker()
{
}

void Worker::start()
{
	thread thread(parseEmail, emailFiles);
	this->workerThread = move(thread);
}
