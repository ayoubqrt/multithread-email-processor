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

list<string> possibleTags = list<string>{
		tagSender,
		tagRecipientTo,
		tagRecipientCc,
		tagRecipientBcc,
		tagMessageId,
		tagDate,
		tagFrom,
		tagTo,
		tagSubject,
		tagMimeVersion,
		tagContentType,
		tagContentTransferEncoding,
		tagXFrom};

void findWithArray(const string &line, const list<string> &tags, string &tagToReturn, size_t &positionEndTag)
{
	for (const auto &tag : tags)
	{
		size_t pos = line.find(tag);
		if (pos != string::npos)
		{
			positionEndTag = pos + tag.size();
			tagToReturn = tag;
			return;
		}
	}
}

bool find_tag(string line, string &tag, size_t &positionEndTag)
{
	findWithArray(line, possibleTags, tag, positionEndTag);
	bool isTag = positionEndTag > 0;

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

bool comparePaths(const filesystem::path &p1, const filesystem::path &p2)
{
	return p1.filename() < p2.filename();
}

void getRecipients(const string &line, stringstream &recipients)
{
	stringstream ss(line);
	string recipient;
	while (getline(ss, recipient, ','))
	{
		trim(recipient);
		if (recipient.size() > 0)
		{
			recipients << recipient << "\n";
		}
	}
}

vector<thread::id> workingThreadsIds;

map<string, vector<string>> emailAdresses = map<string, vector<string>>();

void writeInFile(stringstream &recipients, string senderMail, string threadId)
{
	ofstream threadFile;
	threadFile.open("threads/" + threadId + "/" + senderMail + ".txt", fstream::app);

	threadFile << recipients.rdbuf();

	threadFile.close();
}

void parseEmail(const vector<string> &emailFiles)
{
	thread::id currentThreadId = this_thread::get_id();
	std::ostringstream ss;
	ss << currentThreadId;
	string threadIdStr = ss.str();

	filesystem::create_directory("threads/" + threadIdStr);
	bool isAnyRecipients = false;
	for (const auto &email_file : emailFiles)
	{
		ifstream file(email_file);
		string line;

		string sender = "";
		stringstream recipients;

		string lastTag = "";

		while (getline(file, line))
		{
			if (line.find("X-From:") == 0)
			{
				break;
			}
			std::smatch m;

			size_t positionEndTag = 0;
			string tag = "";

			bool isTag = find_tag(line, tag, positionEndTag);

			if (isTag)
			{
				lastTag = tag;

				if (isTagSender(tag))
				{

					sender = line.substr(positionEndTag);
					trim(sender);
				}
				else if (isTagRecipient(tag))
				{
					string recipientsLine = line.substr(positionEndTag);
					getRecipients(recipientsLine, recipients);
					isAnyRecipients = true;
				}
			}
			else if (isTagRecipient(lastTag))
			{
				getRecipients(line, recipients);
			}
		}

		file.close();

		if (sender == "" || !isAnyRecipients)
			continue;

		writeInFile(recipients, sender, threadIdStr);
	}
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

	for (const auto &entry : filesystem::recursive_directory_iterator("./maildircomplete"))
	{
		if (entry.is_regular_file())
		{
			emailFiles.push_back(entry.path().string());
		}
	}

	int numberEmailsForEachThread = (int)(emailFiles.size() / nbThreads);

	for (int i = 0; i < nbThreads; i++)
	{
		int start = i * (numberEmailsForEachThread);
		int end = start + numberEmailsForEachThread - 1;
		if (i == nbThreads - 1)
			end = emailFiles.size() - 1;

		vector<string> subPart = slicing(emailFiles, start, end);
		workers.push_back(thread(parseEmail, subPart));
	}

	for (auto &worker : workers)
	{
		worker.join();
	}

	parseThreadsResults();
	cout << "Done" << endl;

	return 0;
}
