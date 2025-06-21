#include "GroceryTracker.h"
#include <fstream>
#include <iostream>

// Read item names from the input file and count the frequency
void GroceryTracker::LoadItemsFromFile(const std::string& filename) {
	std::ifstream file(filename);
	std::string item;

	if (!file.is_open()) {
		std::cerr << "Error opening input file.\n";
		return;
	}

	while (file >> item) {
		++itemFrequency[item];
	}

	file.close();
}

// Write item/frequency pairs to a backup file
void GroceryTracker::SaveFrequenciesToFile(const std::string& filename) const {
	std::ofstream outFile(filename);

	for (const auto& pair : itemFrequency) {
		outFile << pair.first << " " << pair.second << "\n";
	}

	outFile.close();
}

// Return the frequency of a specific item
int GroceryTracker::GetItemFrequency(const std::string& item) const {
	auto it = itemFrequency.find(item);
	if (it != itemFrequency.end()) {
		return it->second;
	}
	return 0;
}

// Display all item frequencies
void GroceryTracker::PrintAllFrequencies() const {
	for (const auto& pair : itemFrequency) {
		std::cout << pair.first << " " << pair.second << "\n";
	}
}

// Display a histogram using asterisks
void GroceryTracker::PrintHistogram() const {
	for (const auto& pair : itemFrequency) {
		std::cout << pair.first << " ";
		for (int i = 0; i < pair.second; ++i) {
			std::cout << "*";
		}
		std::cout << "\n";
	}
}