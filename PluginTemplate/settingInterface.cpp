
#include "interface.h"
#include "settingInterface.h"

settingInterface* settingInterface::instence = 0;

bool settingInterface::set(const char* func) {
	char buf[MAX_PATH];
	int iLength = 0;

	iLength = strlen(func);

	if (strlen(func) > MAX_PATH - 4) {
		return 0;
	}

	memset(buf, 0, MAX_PATH);
	sprintf(buf, "%s %s", "bp", func);

	return DbgCmdExecDirect(buf);
}

bool settingInterface::set(const std::string func) {
	char buf[MAX_PATH];

	if (func.length() > MAX_PATH - 4) {
		return 0;
	}

	memset(buf, 0, MAX_PATH);
	sprintf(buf, "%s %s", "bp", func.c_str());

	return DbgCmdExecDirect(buf);
}

bool settingInterface::clear(const char* func) {
	char buf[MAX_PATH];
	int iLength = 0;

	iLength = strlen(func);

	if (strlen(func) > MAX_PATH - 4) {
		return 0;
	}

	memset(buf, 0, MAX_PATH);
	sprintf(buf, "%s %s", "bc", func);

	return DbgCmdExecDirect(buf);
}

bool settingInterface::clear(const std::string func) {
	char buf[MAX_PATH];

	if (func.length() > MAX_PATH - 4) {
		return 0;
	}

	memset(buf, 0, MAX_PATH);
	sprintf(buf, "%s %s", "bc", func.c_str());

	return DbgCmdExecDirect(buf);
}

std::list<std::string> settingInterface::setArray(std::list<std::string> &funcArr) {
	std::list<std::string> unsucessFunc;

	for (auto x = funcArr.begin(); x != funcArr.end(); x++) {
		if (!set(*x)) {
			unsucessFunc.push_back(*x);
		}
	}

	return unsucessFunc;
}

std::list<std::string> settingInterface::clearArray(std::list<std::string> &funcArr) {
	std::list<std::string> unsucessFunc;

	for (auto x = funcArr.begin(); x != funcArr.end(); x++) {
		if (!clear(*x)) {
			unsucessFunc.push_back(*x);
		}
	}

	return unsucessFunc;
}