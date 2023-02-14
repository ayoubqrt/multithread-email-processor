#include <iostream>
#include <vector>
#include <sstream>
#include <list>

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

bool isTagRecipient(string tag);
bool isTagSender(string tag);
void find_recipients(const string &line, stringstream &recipients);
void parseEmail(const vector<string> &emailFiles);
void writeInFile(stringstream &recipients, string senderMail, string threadId);