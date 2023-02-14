#include <include/Worker.h>
#include <include/Mail.h>
Worker::Worker(TaskType taskType, vector<string> emailFiles)
{
	this->taskType = taskType;
	this->emailFiles = emailFiles;

	thread myThread = thread(parseEmail, emailFiles);
	this->workerThread = move(myThread);
}

// Worker::~Worker()
// {
// }

void Worker::start()
{
	this->workerThread.join();
}
