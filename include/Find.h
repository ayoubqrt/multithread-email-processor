#pragma once

#include <iostream>
#include <list>
#include <string>

using namespace std;

static string tagSender = "From:";
static string tagRecipientTo = "To:";
static string tagRecipientCc = "Cc:";
static string tagRecipientBcc = "Bcc:";

static string tagMessageId = "Message-ID:";
static string tagDate = "Date:";
static string tagFrom = "From:";
static string tagTo = "To:";
static string tagSubject = "Subject:";
static string tagMimeVersion = "Mime-Version:";
static string tagContentType = "Content-Type:";
static string tagContentTransferEncoding = "Content-Transfer-Encoding:";
static string tagXFrom = "X-From:";

static list<string> possibleTags = list<string>{
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

using namespace std;

void findWithArray(const string &line, const list<string> &tags, string &tagToReturn, size_t &positionEndTag);
bool find_tag(string line, string &tag, size_t &positionEndTag);
