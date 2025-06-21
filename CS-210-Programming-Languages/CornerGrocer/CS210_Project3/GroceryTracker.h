// The GroceryTracker class handles item frequency tracking from an input file.

#ifndef GROCERY_TRACKER_H
#define GROCERY_TRACKER_H

#include <string>
#include <map>

class GroceryTracker {
private:
	// Store item names and frequencies
	std::map<std::string, int> itemFrequency;

public:
	void LoadItemsFromFile(const std::string& filename);
	void SaveFrequenciesToFile(const std::string& filename) const;
	int GetItemFrequency(const std::string& item) const;
	void PrintAllFrequencies() const;
	void PrintHistogram() const;
};

#endif
