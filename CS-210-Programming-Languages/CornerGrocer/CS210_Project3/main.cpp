#include "GroceryTracker.h"
#include <iostream>

// Display main menu options
void ShowMenu() {
    std::cout << "\n=== Corner Grocer Menu ===\n";
    std::cout << "1. Search for an item\n";
    std::cout << "2. Display item frequencies\n";
    std::cout << "3. Display histogram\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter your choice: ";
}

int main() {
    GroceryTracker tracker;
    tracker.LoadItemsFromFile("Input_File.txt");
    tracker.SaveFrequenciesToFile("frequency.dat");

    int choice;
    std::string item;

    // Menu loop
    while (true) {
        ShowMenu();
        std::cin >> choice;

        switch (choice) {
        case 1: // Search for an item by name
            std::cout << "Enter item name: ";
            std::cin >> item;
            std::cout << item << " was purchased " << tracker.GetItemFrequency(item) << " times.\n";
            break;
        case 2: // Display all frequencies
            tracker.PrintAllFrequencies();
            break;
        case 3: // Display histrogram
            tracker.PrintHistogram();
            break;
        case 4: // Exit program
            std::cout << "Exiting program.\n";
            return 0;
        default: // Input validation
            std::cout << "Invalid option. Try again.\n";
            break;
        }
    }

    return 0;
}
