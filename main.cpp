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
#include <regex>

using namespace std;

string begginingLineSender = "From:";
string begginingLineRecipient = "To:";
string begginingLineRecipientCc = "Cc:";
string begginingLineRecipientBcc = "Bcc:";

bool isTagRecipient(string line)
{
	return line.find(begginingLineRecipient) == 0 || line.find(begginingLineRecipientCc) == 0 || line.find(begginingLineRecipientBcc) == 0;
}

// trim from start (in place)
static inline void ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
																	std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
											 std::not1(std::ptr_fun<int, int>(std::isspace)))
							.base(),
					s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
	rtrim(s);
	ltrim(s);
}

static inline std::string trim_copy(std::string s)
{
	trim(s);
	return s;
}

void getRecipients(const string &line, vector<string> &recipients)
{
	stringstream ss(line);
	string recipient;
	while (getline(ss, recipient, ','))
	{
		trim(recipient);
		recipients.push_back(recipient);
	}
}

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

	for (const auto &email_file : email_files)
	{
		cout << "Processing " << email_file << endl;

		ifstream file(email_file);
		string line;

		string sender = "";
		vector<string> recipients;

		bool oldConversationFounded = false;
		string lastTag = "";

		while (!oldConversationFounded && getline(file, line))
		{
			if (line.find("X-From:") == 0)
			{
				break;
			}
			std::smatch m;

			regex_search(line, m, regex("^[a-zA-Z]*-?[a-zA-Z]*:"));
			// size_t positionEndTag = line.find(":");
			// bool isTag = positionEndTag == 0;
			bool isTag = m.position(0) == 0;

			int positionEndTag = m.position(0) + m.length(0);

			cout << "line: " << line << endl;
			cout << "isTag: " << isTag << endl;
			cout << "lastTag: " << lastTag << endl;
			cout << "positionEndTag: " << positionEndTag << endl;

			if (isTag)
			{
				lastTag = m[0];

				if (line.find(begginingLineSender) == 0)
				{
					sender = line.substr(positionEndTag);
				}
				else if (isTagRecipient(line))
				{
					string line = line.substr(positionEndTag);
					stringstream ss(line.substr(positionEndTag));
					string recipient;
					while (getline(ss, recipient, ','))
					{
						trim(recipient);
						recipients.push_back(recipient);
					}
				}
			}
			else if (lastTag == begginingLineRecipient || lastTag == begginingLineRecipientCc || lastTag == begginingLineRecipientBcc)
			{
				stringstream ss(line);
				string recipient;
				while (getline(ss, recipient, ','))
				{
					trim(recipient);
					if (recipient.size() > 0)
					{
						recipients.push_back(recipient);
					}
				}
			}
			cout << " ---- " << endl;

			// cout << oldConversationFounded << endl;
			/*
			if (line.find(begginingLineSender) == 0)
			{
				sender = line.substr(begginingLineSender.size());
			}
			else if (line.find(begginingLineRecipient) == 0)
			{
				stringstream ss(line.substr(begginingLineRecipient.size()));
				string recipient;
				while (getline(ss, recipient, ','))
				{
					recipients.push_back(recipient);
				}
			}
			else if (line.find(begginingLineRecipientCc) == 0)
			{
				stringstream ss(line.substr(begginingLineRecipientCc.size()));
				string recipient;
				while (getline(ss, recipient, ','))
				{
					recipients.push_back(recipient);
				}
			}
			else if (line.find(begginingLineRecipientBcc) == 0)
			{
				stringstream ss(line.substr(begginingLineRecipientBcc.size()));
				string recipient;
				while (getline(ss, recipient, ','))
				{
					recipients.push_back(recipient);
				}
			}
			*/

			// if (sender != "" && recipients.size() > 0)
			// 	oldConversationFounded = true;

			// if (line.find("Original Message") != -1)
			// {
			// 	oldConversationFounded = true;
			// }
		}

		cout << "Sender: " << sender << endl;
		cout << "Recipients:" << endl;
		for (const auto &recipient : recipients)
		{
			cout << recipient << endl;
		}
		file.close();

		if (sender == "")
			continue;

		writeInFile(recipients, sender, currentThreadId);
	}
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
	vector<thread> workers;
	vector<string> email_files = {};
	int nbThreads = 8;

	for (const auto &entry : filesystem::recursive_directory_iterator("./maildir"))
	{
		if (entry.is_regular_file())
		{
			email_files.push_back(entry.path().string());
		}
	}
	int numberEmailsForEachThread = email_files.size() / 1;

	for (int i = 0; i < nbThreads; i++)
	{
		int start = i * (numberEmailsForEachThread - 1);
		int end = (i + 1) * (numberEmailsForEachThread - 1);
		if (i == nbThreads - 1)
			end = email_files.size() - 1;

		vector<string> subPart = slicing(email_files, start, end);
		workers.push_back(thread(parseEmail, subPart));
	}

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
