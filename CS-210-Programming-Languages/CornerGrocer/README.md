A command-line C++ application that reads a daily sales file and reports how often each grocery item was purchased.

## Features
1. **Full Item Report** – Display every unique item with its purchase count.  
2. **Single Item Lookup** – Query the count for any specific item.  
3. **Histogram View** – Print a “*” bar graph of all item frequencies.  
4. **Persistence** – Write today’s frequency table to `frequency.dat` for backup.  
5. **Robust I/O** – Graceful handling of bad filenames or malformed records.

## What Went Well
• Clean, single-responsibility `GroceryTracker` class encapsulates
all counting logic.
• Comprehensive error checking for file opens, bad lines, and menu input.
• Consistent naming & inline comments for readability.

## Potential Enhancements
• Stronger menu validation (reject negatives, empty strings, etc.).
• Case-insensitive searches and counting (Apple ≡ apple).
• Stream processing for very large files instead of full vector load.

## Academic Integrity
This project was completed individually for CS-210 Programming Languages at
Southern New Hampshire University. It is provided for portfolio review only.
Do not submit this code for credit in any course.
