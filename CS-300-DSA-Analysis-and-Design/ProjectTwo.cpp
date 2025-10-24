//============================================================================
// Name        : Final.cpp
// Author      : Anthony Arnett Oct 23, 2025
// Version     : 1.0
//============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// simple record for a course
struct Course
{
	string courseId;
	string courseName;
	vector<string> prereqs;
};

// trim helpers
static inline void ltrim(string &s)
{
	size_t i = 0;
	while (i < s.size() && isspace(static_cast<unsigned char>(s[i])))
		++i;
	s.erase(0, i);
}
static inline void rtrim(string &s)
{
	if (s.empty())
		return;
	size_t i = s.size();
	while (i > 0 && isspace(static_cast<unsigned char>(s[i - 1])))
		--i;
	s.erase(i);
}
static inline void trim(string &s)
{
	ltrim(s);
	rtrim(s);
}

// make lookups case-insensitive by normalizing ids
static inline void to_upper(string &s)
{
	for (char &c : s)
		c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
}

// split a line by commas
static vector<string> split_csv(const string &line)
{
	vector<string> out;
	string token;
	stringstream ss(line);
	while (getline(ss, token, ','))
	{
		trim(token);
		out.push_back(token);
	}
	return out;
}

// bst node and tree
struct Node
{
	Course course;
	Node *left;
	Node *right;
	Node() : left(nullptr), right(nullptr) {}
	Node(const Course &c) : course(c), left(nullptr), right(nullptr) {}
};

class BinarySearchTree
{
public:
	BinarySearchTree() : root(nullptr) {}
	~BinarySearchTree() { destroy(root); }

	// insert by course id
	void Insert(const Course &c) { insert(root, c); }

	// in order print gives sorted order by course id
	void InOrder() const { inOrder(root); }

	bool Empty() const { return root == nullptr; }

	// search by id returns true if found
	bool Search(string id, Course &out) const
	{
		to_upper(id);
		Node *cur = root;
		while (cur)
		{
			if (id == cur->course.courseId)
			{
				out = cur->course;
				return true;
			}
			if (id < cur->course.courseId)
				cur = cur->left;
			else
				cur = cur->right;
		}
		return false;
	}

private:
	Node *root;

	static void destroy(Node *n)
	{
		if (!n)
			return;
		destroy(n->left);
		destroy(n->right);
		delete n;
	}

	static void insert(Node *&n, const Course &c)
	{
		if (!n)
		{
			n = new Node(c);
			return;
		}
		if (c.courseId < n->course.courseId)
		{
			insert(n->left, c);
		}
		else if (c.courseId > n->course.courseId)
		{
			insert(n->right, c);
		}
		else
		{
			// same id again, replace with the newest record
			n->course = c;
		}
	}

	static void inOrder(Node *n)
	{
		if (!n)
			return;
		inOrder(n->left);
		cout << n->course.courseId << ", " << n->course.courseName << endl;
		inOrder(n->right);
		// no extra blank lines
	}
};

// File loading
// reads courses from csv and inserts into bst
static size_t loadCoursesFromFile(const string &path, BinarySearchTree &bst)
{
	ifstream in(path);
	if (!in.is_open())
	{
		cout << "Could not open file." << endl;
		return 0;
	}

	size_t count = 0;
	string line;
	while (getline(in, line))
	{
		trim(line);
		if (line.empty())
			continue;

		auto fields = split_csv(line);
		// need at least id and title
		if (fields.size() < 2)
			continue;

		Course c;
		c.courseId = fields[0];
		to_upper(c.courseId);
		c.courseName = fields[1];

		// any other fields are prereqs
		for (size_t i = 2; i < fields.size(); ++i)
		{
			if (!fields[i].empty())
			{
				string pid = fields[i];
				to_upper(pid);
				c.prereqs.push_back(pid);
			}
		}

		bst.Insert(c);
		++count;
	}

	cout << count << " courses read" << endl;
	return count;
}

// prints one course and its prereqs
static void printCourse(const Course &c, const BinarySearchTree &bst)
{
	cout << c.courseId << ", " << c.courseName << endl;
	cout << "Prerequisites: ";
	if (c.prereqs.empty())
	{
		cout << "None" << endl;
		return;
	}

	for (size_t i = 0; i < c.prereqs.size(); ++i)
	{
		const string &pid = c.prereqs[i];
		Course pc;
		if (bst.Search(pid, pc))
		{
			cout << pid << " (" << pc.courseName << ")";
		}
		else
		{
			// if the prereq course is not present in the file, still show the id
			cout << pid;
		}
		if (i + 1 < c.prereqs.size())
			cout << ", ";
	}
	cout << endl;
}

int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	BinarySearchTree catalog;
	bool running = true;

	cout << "Welcome to the course planner.\n"
			 << endl;

	while (running)
	{
		// menu matches the required options and simple wording
		cout << "1. Load Data Structure." << endl;
		cout << "2. Print Course List." << endl;
		cout << "3. Print Course." << endl;
		cout << "9. Exit\n"
				 << endl;

		cout << "What would you like to do? ";

		string choiceRaw;
		if (!(cin >> choiceRaw))
		{
			// if input stream fails just stop
			return 0;
		}

		// only allow digits
		bool numeric = !choiceRaw.empty() && all_of(choiceRaw.begin(), choiceRaw.end(), ::isdigit);
		int choice = numeric ? stoi(choiceRaw) : -1;
		cout << endl;

		switch (choice)
		{
		case 1:
		{
			// load csv into bst
			cout << "Enter the file name: ";
			string path;
			if (!(cin >> path))
			{
				cout << "\nNo file provided.\n"
						 << endl;
				break;
			}
			cout << endl;
			loadCoursesFromFile(path, catalog);
			cout << endl;
			break;
		}

		case 2:
		{
			// print all courses sorted by id
			if (catalog.Empty())
			{
				cout << "No data loaded. Please load the data structure first.\n"
						 << endl;
			}
			else
			{
				cout << "Here is a sample schedule:\n"
						 << endl;
				catalog.InOrder();
				cout << endl;
			}
			break;
		}

		case 3:
		{
			// look up one course and show its info
			if (catalog.Empty())
			{
				cout << "No data loaded. Please load the data structure first.\n"
						 << endl;
				break;
			}
			cout << "What course do you want to know about? ";
			string id;
			if (!(cin >> id))
			{
				cout << "\nInvalid input.\n"
						 << endl;
				break;
			}
			to_upper(id);
			cout << endl;

			Course c;
			if (catalog.Search(id, c))
			{
				printCourse(c, catalog);
				cout << endl;
			}
			else
			{
				// clear message if the id is not in the tree
				cout << "Course " << id << " not found.\n"
						 << endl;
			}
			break;
		}

		case 9:
			// exit path
			cout << "Thank you for using the course planner!" << endl;
			running = false;
			break;

		default:
			// keep the invalid selection message short and clear
			cout << choiceRaw << " is not a valid option.\n"
					 << endl;
			break;
		}
	}

	return 0;
}
