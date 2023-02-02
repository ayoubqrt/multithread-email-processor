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

mutex mtx; // Mutex pour synchroniser l'accès aux résultats
unordered_map<string, unordered_map<string, int>> results;

void writeInFile(vector<string> recipients, string senderMail, string threadId)
{
	ofstream threadFile;
	filesystem::create_directory("threads/" + threadId);
	threadFile.open("threads/" + threadId + "/" + senderMail + ".txt", fstream::app);

	for (const auto &recipient : recipients)
	{
		threadFile << recipient << endl;
	}
	threadFile.close();
}

void process_email(string file_name, int threadId)
{
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
	writeInFile(recipients, sender, to_string(threadId));
	// Ajouter les résultats à la map partagée
	// lock_guard<mutex> guard(mtx);
	// for (auto recipient : recipients)
	// {
	// 	results[sender][recipient]++;
	// }
}

int main()
{
	int threadId = 0;
	queue<string> email_files = {};
	for (const auto &entry : filesystem::recursive_directory_iterator("./maildir"))
	{
		email_files.push(entry.path().string());
	}

	// list<thread> workers = list<thread>(16);

	vector<thread> workers;

	// Créer un thread pour chaque e-mail
	// for (auto email_file : email_files)
	// {
	// 	workers.push_back(thread(process_email, email_file));
	// }

	// create a thread for each mail but limit of 16 threads and wait for the end of the threads and then create new threads
	while (email_files.size() != 0)
	{
		if (workers.size() != 16)
		{
			// threadId++;
			// thread myThread =

			workers.push_back(thread(process_email, email_files.front(), threadId));

			// workers.insert(pair<thread, int>(myThread, threadId));
			// workers.insert(pair<thread, int>(myThread, threadId));
			// workers.insert(, threadId);
			email_files.pop();
		}

		// wait for the end of the threads
		for (auto it = workers.begin(); it != workers.end(); it++)
		{
			// cout << "size: " << workers.size() << endl;
			if (it->joinable())
			{
				cout << "joinable" << endl;
				it->detach();
				// workers.erase(it);
				break;
			}
		}

		// for (auto it = workers.begin(); it != workers.end(); it++)
		// {
		// 	if (it->joinable())
		// 	{
		// 		it->join();
		// 		workers.erase(it);
		// 		break;
		// 	}
		// }
	}

	// Attente de la fin des threads
	for (auto &worker : workers)
	{
		worker.join();
	}

	cout << "Done" << endl;

	// Enregistrement des résultats dans un fichier
	ofstream output_file("results.txt");
	for (auto sender : results)
	{
		output_file << sender.first << ": ";
		for (auto recipient : sender.second)
		{
			output_file << recipient.second << ":" << recipient.first << " ";
		}
		output_file << endl;
	}
	output_file.close();

	return 0;
}