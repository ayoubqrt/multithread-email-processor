#include <include/Mail.h>
#include <include/Trim.h>
#include <include/Find.h>

bool isTagRecipient(string tag)
{
	return tag == tagRecipientTo || tag == tagRecipientCc || tag == tagRecipientBcc;
}

bool isTagSender(string tag)
{
	return tag == tagSender;
}

void find_recipients(const string &line, stringstream &recipients)
{
	stringstream ss(line);
	string recipient;
	while (getline(ss, recipient, ','))
	{
		trim(recipient);
		if (recipient.size() > 0)
		{
			// recipients.push_back(recipient);
			recipients << recipient << "\n";
		}
	}
}

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
					find_recipients(recipientsLine, recipients);
					isAnyRecipients = true;
				}
			}
			else if (isTagRecipient(lastTag))
			{
				find_recipients(line, recipients);
			}
		}

		file.close();

		if (sender == "" || !isAnyRecipients)
			continue;

		writeInFile(recipients, sender, threadIdStr);
	}
}