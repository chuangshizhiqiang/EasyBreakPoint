#pragma once
#include <string.h>
#include <list>

class settingInterface {
private:
	static settingInterface* instence;

public:

	bool set(const char* func);
	bool set(const std::string func);
	bool clear(const char* func);
	bool clear(const std::string func);

	std::list<std::string> setArray(std::list<std::string> &funcArr);
	std::list<std::string> clearArray(std::list<std::string> &funcArr);

	static settingInterface* getInstance() {
		if (instence == NULL) {
			instence = new settingInterface();
		}
		return instence;
	}
};