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
#include <bits/stdc++.h>

using namespace std;

string tagSender = "From:";
string tagRecipientTo = "To:";
string tagRecipientCc = "Cc:";
string tagRecipientBcc = "Bcc:";

string tagMessageId = "Message-ID:";
string tagDate = "Date:";
string tagFrom = "From:";
string tagTo = "To:";
string tagSubject = "Subject:";
string tagMimeVersion = "Mime-Version:";
string tagContentType = "Content-Type:";
string tagContentTransferEncoding = "Content-Transfer-Encoding:";
string tagXFrom = "X-From:";

list<string> uselessTags = list<string>{
		tagMessageId,
		tagDate,
		tagFrom,
		tagTo,
		tagSubject,
		tagMimeVersion,
		tagContentType,
		tagContentTransferEncoding,
		tagXFrom};

list<string> tags = list<string>{
		tagSender,
		tagRecipientTo,
		tagRecipientCc,
		tagRecipientBcc};

void findWithArray(const string &line, const list<string> &tags, string &tagToReturn, size_t &positionEndTag)
{
	for (const auto &tag : tags)
	{
		size_t pos = line.find(tag);
		if (pos != string::npos)
		{
			positionEndTag = pos + tag.size();
			return;
		}
	}
}

bool find_tag(string line, string &tag, size_t &positionEndTag)
{
	findWithArray(line, uselessTags, tag, positionEndTag);
	bool isTag = positionEndTag == 0;

	return isTag;
}

bool isTagRecipient(string tag)
{
	return tag == tagRecipientTo || tag == tagRecipientCc || tag == tagRecipientBcc;
}

bool isTagSender(string tag)
{
	return tag == tagSender;
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
		if (recipient.size() > 0)
		{
			recipients.push_back(recipient);
		}
	}
}

vector<thread::id> workingThreadsIds;

map<string, vector<string>> emailAdresses = map<string, vector<string>>();

void writeInFile(vector<string> recipients, string senderMail, string threadId)
{
	ofstream threadFile;
	threadFile.open("threads/" + threadId + "/" + senderMail + ".txt", fstream::app);

	for (const auto &recipient : recipients)
	{
		threadFile << recipient << endl;
	}
	threadFile.close();
}

void parseEmail(const vector<string> &emailFiles)
{
	thread::id currentThreadId = this_thread::get_id();
	std::ostringstream ss;
	ss << currentThreadId;
	string threadIdStr = ss.str();

	filesystem::create_directory("threads/" + threadIdStr);

	for (const auto &email_file : emailFiles)
	{
		// cout << "Processing " << email_file << endl;

		ifstream file(email_file);
		string line;

		string sender = "";
		vector<string> recipients;

		string lastTag = "";

		while (getline(file, line))
		{
			if (line.find("X-From:") == 0)
			{
				break;
			}
			std::smatch m;

			// regex_search(line, m, regex("^[a-zA-Z]*-?[a-zA-Z]*:"));
			// size_t positionEndTag = line.find(":");
			// bool isTag = positionEndTag == 0;
			// bool isTag = m.position(0) == 0;

			size_t positionEndTag = 0;
			string tag = "";

			bool isTag = find_tag(line, tag, positionEndTag);

			// int positionEndTag = m.position(0) + m.length(0);

			// cout << "line: " << line << endl;
			// cout << "isTag: " << isTag << endl;
			// cout << "lastTag: " << lastTag << endl;
			// cout << "positionEndTag: " << positionEndTag << endl;

			if (isTag)
			{
				// string tag = m[0];
				lastTag = tag;

				if (isTagSender(tag))
				{
					// cout << "isTagSender" << endl;

					sender = line.substr(positionEndTag);
					trim(sender);
				}
				else if (isTagRecipient(tag))
				{
					// cout << "isTagRecipient" << endl;
					// cout << "line: " << line << endl;
					// cout << "positionEndTag: " << positionEndTag << endl;
					string recipientsLine = line.substr(positionEndTag);
					getRecipients(recipientsLine, recipients);
				}
			}
			else if (isTagRecipient(lastTag))
			{
				getRecipients(line, recipients);
			}
			// cout << " ---- " << endl;
		}

		// cout << "Sender: " << sender << endl;
		// cout << "Recipients:" << endl;
		// for (const auto &recipient : recipients)
		// {
		// 	cout << recipient << endl;
		// }
		file.close();

		if (sender == "")
			continue;

		writeInFile(recipients, sender, threadIdStr);
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
	vector<string> emailFiles = {};
	int nbThreads = 8;
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	filesystem::create_directory("threads");

	for (const auto &entry : filesystem::recursive_directory_iterator("./maildir"))
	{
		if (entry.is_regular_file())
		{
			emailFiles.push_back(entry.path().string());
		}
	}

	int numberEmailsForEachThread = (int)(emailFiles.size() / nbThreads);

	for (int i = 0; i < nbThreads; i++)
	{
		int start = i * (numberEmailsForEachThread - 1);
		int end = (i + 1) * (numberEmailsForEachThread - 1);
		if (i == nbThreads - 1)
			end = emailFiles.size() - 1;

		vector<string> subPart = slicing(emailFiles, start, end);
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
