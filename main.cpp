#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <vector>
#include <filesystem>
#include <list>
#include <queue>
#include <map>
using namespace std;

// mutex mtx; // Mutex pour synchroniser l'accès aux résultats
// unordered_map<string, unordered_map<string, int>> results;

// queue<thread::id> workingThreadsIds;
// list protected by mutex
// mutex mtx; // Mutex pour synchroniser l'accès aux résultats
vector<thread::id> workingThreadsIds;

map<string, vector<string>> emailAdresses = map<string, vector<string>>();

void writeInFile(vector<string> recipients, string senderMail, thread::id threadId)
{
	std::ostringstream ss;
	ss << threadId;
	string threadIdStr = ss.str();

	ofstream threadFile;
	filesystem::create_directory("threads/" + threadIdStr);
	threadFile.open("threads/" + threadIdStr + "/" + senderMail + ".txt", fstream::app);

	for (const auto &recipient : recipients)
	{
		threadFile << recipient << endl;
	}
	threadFile.close();
}

void parseEmail(const vector<string> &email_files)
{
	thread::id currentThreadId = this_thread::get_id();
	// workingThreadsIds.insert(currentThreadId);
	// workingThreadsIds.push_back(currentThreadId);

	// iterate on the email files
	for (const auto &email_file : email_files)
	{
		cout << "Processing " << email_file << endl;
	}

	/*
	ifstream email_file(file_name);
	string line;

	// Variables pour stocker les informations de l'e-mail
	string sender;
	vector<string> recipients;

	cout << "Processing " << file_name << endl;

	while (getline(email_file, line))
	{
		if (line.find("Original Message") != -1)
			break;
		if (line.find("From: ") == 0)
		{
			// From: Dean Russell on 01/31/2001 10:49 AM
			sender = line.substr(6); // Extraire l'expéditeur
		}
		else if (line.find("To: ") == 0)
		{
			stringstream ss(line.substr(4));
			string recipient;
			while (getline(ss, recipient, ','))
			{
				recipients.push_back(recipient); // Ajouter les destinataires directs
			}
		}
		else if (line.find("Cc: ") == 0)
		{
			stringstream ss(line.substr(4));
			string recipient;
			while (getline(ss, recipient, ','))
			{
				recipients.push_back(recipient); // Ajouter les destinataires en copie
			}
		}
		else if (line.find("Bcc: ") == 0)
		{
			stringstream ss(line.substr(5));
			string recipient;
			while (getline(ss, recipient, ','))
			{
				recipients.push_back(recipient); // Ajouter les destinataires en copie cachée
			}
		}
	}

	cout << "Sender: " << sender << endl;
	email_file.close();
	writeInFile(recipients, sender, currentThreadId);
	// Ajouter les résultats à la map partagée
	// lock_guard<mutex> guard(mtx);
	// for (auto recipient : recipients)
	// {
	// 	results[sender][recipient]++;
	// }

	lock_guard<mutex> guard(mtx);
	for (int i = 0; i < workingThreadsIds.size(); i++)
	{
		if (workingThreadsIds[i] == currentThreadId)
		{
			workingThreadsIds.erase(workingThreadsIds.begin() + i);
		}
	}*/
}

vector<string> slicing(vector<string> &arr, int X, int Y)
{
	auto start = arr.begin() + X;
	auto end = arr.begin() + Y + 1;

	vector<string> result(Y - X + 1);

	copy(start, end, result.begin());

	return result;
}

int main()
{
	int threadId = 0;
	vector<string> email_files = {};
	for (const auto &entry : filesystem::recursive_directory_iterator("./maildir"))
	{
		if (entry.is_regular_file())
		{
			email_files.push_back(entry.path().string());
		}
	}

	// list<thread> workers = list<thread>(16);

	vector<thread> workers;

	// Créer un thread pour chaque e-mail
	// for (auto email_file : email_files)
	// {
	// 	workers.push_back(thread(parseEmail, email_file));
	// }

	// create a thread for each mail but limit of 16 threads and wait for the end of the threads and then create new threads
	// bool isAllEmailsParsed = email_files.size() == 0;

	// while (!isAllEmailsParsed)
	// {
	// 	if (workers.size() != 16)
	// 	{
	// 		// threadId++;
	// 		// thread myThread =
	// 		string emailFile = email_files.front();
	// 		// thread myThread = thread(parseEmail, emailFile, threadId);

	// 		// create thread
	// 		thread worker(parseEmail, emailFile);
	// 		workers.push_back(std::move(worker));

	// 		// workers.insert(pair<thread, int>(myThread, threadId));
	// 		// workers.insert(pair<thread, int>(myThread, threadId));
	// 		// workers.insert(, threadId);
	// 		email_files.pop();
	// 	}

	// wait for the end of the threads
	// for (auto it = workers.begin(); it != workers.end(); it++)
	// {
	// 	// cout << "size: " << workers.size() << endl;
	// 	for (int i = 0; i < workingThreadsIds.size(); i++)
	// 	{
	// 		if (workingThreadsIds[i] == it->get_id())
	// 		{
	// 			// workingThreadsIds.erase(workingThreadsIds.begin() + i);
	// 			it->detach();
	// 		}
	// 	}
	// }

	// for (auto it = workers.begin(); it != workers.end(); it++)
	// {
	// 	if (it->joinable())
	// 	{
	// 		it->join();
	// 		workers.erase(it);
	// 		break;
	// 	}
	// }
	// }

	int numberEmailsForEachThread = email_files.size() / 8;

	// for (auto email_file : email_files)
	// {

	// 	workers.push_back(thread(parseEmail, email));
	// }

	for (int i = 0; i < 8; i++)
	{
		vector<string> subPart = slicing(email_files, i * numberEmailsForEachThread, (i + 1) * numberEmailsForEachThread);
		workers.push_back(thread(parseEmail, subPart));
	}

	// Attente de la fin des threads
	for (auto &worker : workers)
	{
		worker.join();
	}

	cout << "Done" << endl;

	// Enregistrement des résultats dans un fichier
	// ofstream output_file("results.txt");
	// for (auto sender : results)
	// {
	// 	output_file << sender.first << ": ";
	// 	for (auto recipient : sender.second)
	// 	{
	// 		output_file << recipient.second << ":" << recipient.first << " ";
	// 	}
	// 	output_file << endl;
	// }
	// output_file.close();

	return 0;
}
