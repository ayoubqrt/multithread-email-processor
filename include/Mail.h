#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <list>

using namespace std;

bool isTagRecipient(string tag);
bool isTagSender(string tag);
bool find_recipients(const string &line, stringstream &recipients);
void parseEmail(const vector<string> &emailFiles);
void writeInFile(stringstream &recipients, string senderMail, string threadId);