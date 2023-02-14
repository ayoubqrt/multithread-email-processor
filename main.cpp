#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <filesystem>
#include <list>
#include <queue>
#include <map>
#include <bits/stdc++.h>

#include <include/Mail.h>
#include <include/Find.h>
#include <include/Vector.h>
#include <include/Trim.h>
#include <include/Worker.h>

using namespace std;

bool comparePaths(const filesystem::path &p1, const filesystem::path &p2)
{
	return p1.filename() < p2.filename();
}

void parseThreadsResults()
{
	queue<filesystem::path> filesToProcess;
	vector<filesystem::path> files;
	set<string> senders;

	ofstream output_file("results.txt");

	for (const auto &entry : filesystem::recursive_directory_iterator("./threads"))
	{
		if (entry.is_regular_file())
		{
			files.push_back(entry.path());
		}
	}

	sort(files.begin(), files.end(), comparePaths);

	for (const auto &file : files)
	{
		filesToProcess.push(file);
		senders.insert(file.filename().string());
	}

	for (const auto &senderName : senders)
	{
		map<string, int> recipients = map<string, int>();

		while (!filesToProcess.empty())
		{
			filesystem::path entry = filesToProcess.front();

			if (entry.filename().string() == senderName)
			{
				ifstream file(entry);
				string line;
				while (getline(file, line))
				{
					if (recipients[line] > 0)
					{
						recipients[line] += 1;
					}
					else
					{
						recipients[line] = 1;
					}
				}
				filesToProcess.pop();
			}
			else
			{
				break;
			}
		}

		output_file << senderName << ": ";
		for (const auto &recipient : recipients)
		{
			output_file << " " << recipient.second << ":" << recipient.first;
		}
		output_file << endl;
	}
}

int main()
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	const auto nbThreadsToExecute = std::thread::hardware_concurrency();
	vector<shared_ptr<Worker>> workers;
	vector<string> emailFiles = {};

	filesystem::create_directory("threads");

	for (const auto &entry : filesystem::recursive_directory_iterator("./maildir"))
	{
		if (entry.is_regular_file())
		{
			emailFiles.push_back(entry.path().string());
		}
	}

	int numberEmailsForEachThread = (int)(emailFiles.size() / nbThreadsToExecute);

	for (int i = 0; i < nbThreadsToExecute; i++)
	{
		int start = i * (numberEmailsForEachThread);
		int end = start + numberEmailsForEachThread - 1;
		if (i == nbThreadsToExecute - 1)
			end = emailFiles.size() - 1;

		vector<string> subPart = slicing(emailFiles, start, end);

		auto workerPtr = make_shared<Worker>(TaskType::PARSE_EMAIL, subPart);

		workers.push_back(workerPtr);
	}

	for (auto &worker : workers)
	{
		worker->start();
	}

	parseThreadsResults();

	cout << "Done" << endl;

	return 0;
}
