//This code was completed by Brendan Murray on December 5th 2018 for ENSC 251 taught by Karol Swietlicki
//Student Number: 301335362
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include "header_file.h"
//Define some global constants used later
#define TERMINAL_ELEMENT_WIDTH 10
#define TERMINAL_MAX_ELEMENT_COUNT 8
#define TERMINAL_MIN_WIDTH TERMINAL_ELEMENT_WIDTH * TERMINAL_MAX_ELEMENT_COUNT
//Main driver program
int main()
{
	ifstream mfile;
	string str;
	string filename;
	string POSITIVECLASSIFICATION;
	vector<string> string_parts;
	vector<line> lines;
	vector<int> attribute_indexes;
	//Taking input of file name from user
	cout << "Input the name of the file containing CSV formatted data:" << endl;
	cin >> filename;

	unsigned int i;
	mfile.open(filename);
	//If opening fails, end program
	if (!mfile.is_open())
		return 0;

	//Reading the file
	while (!mfile.eof())
	{
		getline(mfile, str);
		string_parts = split(str, ',');
		line _line;

		if (string_parts.size() == 0)
			continue;

		for (i = 0; i < string_parts.size() - 1; i++) {
			_line.attribute_values.push_back(strtod(string_parts[i].c_str(), NULL));
		}
		_line.classification = string_parts[i];
		lines.push_back(_line);
	}

	//Finding our two classifications
	POSITIVECLASSIFICATION = lines[0].classification;

	//Filling our indexes vector
	for (i = 0; i < lines[0].attribute_values.size(); i++) {
		attribute_indexes.push_back(i);
	}
	//Creating a new node first node
	node *hnode = new node();
	//Breaking that node into the leaves of the entire tree
	hnode = build_tree(hnode, lines, attribute_indexes, POSITIVECLASSIFICATION);

	//Tree info
	cout << endl << "If Tree is not displaying properly, please increase termminal width or edit line 9 of this code to better fit your machine." << endl;
	//legend for the tree
	cout << endl << "Tree Legend :" << endl << "[attr x] : Shown on decision markers, it is the attribute used to make the split." << endl;
	cout << "Floating Point Number : Shown on decision markers, it is the exact number where the decision to split was made in the attribute." << endl;
	cout << "[x] : Number of items in leaf." << endl << "Iris Setosa OR Iris Verticolor : Indicates the entire leaf is composed of the aforementioned type." << endl << endl << endl;
	//Printing tree
	output_tree(hnode);

	return 0;
}

//Splits each line at the delim character which in our case is a , for CSV files
vector<string> split(const string& str, const char& delim) {
	vector<string> result;
	string _temp;
	//this loops breaks when the string is at the end, and iterates through the string
	for (string::const_iterator it = str.begin(); it != str.end(); it++) {
		//if the current char is the delim
		if (*it == delim) {
			//if temp is not empty, add it to the result vector, then clear it
			if (!_temp.empty()) {
				result.push_back(_temp);
				_temp.clear();
			}
		}
		//if temp is empty, store the value in the string iterator
		else {
			_temp += *it;
		}
	}
	//if the temp is not empty add it to the result vector
	if (!_temp.empty()) {
		result.push_back(_temp);
	}
	return result;
}

//part of calculating the entropy for each split in the tree
double get_entropy(vector<line> lines, string POSITIVECLASSIFICATION) {
	unsigned int i, positive_count = 0;
	double entropy = 0;
	//for each line count if it is our first classification, we called it positive
	for (i = 0; i < lines.size(); i++) {
		if (lines[i].classification == POSITIVECLASSIFICATION)
			positive_count++;
	}
	//utilizing the formulas to calculate entropy
	double positive_proportion = (double)positive_count / lines.size();
	double negative_proportion = (double)(lines.size() - positive_count) / lines.size();
	entropy = 0 - positive_proportion * log_2(positive_proportion) - negative_proportion * log_2(negative_proportion);
	return entropy;
}


//computing the info gained by the change in entropy
double compute_info_gain_entropy_delta(vector<line> lines, double threshold, int index, string POSITIVECLASSIFICATION) {
	unsigned int i;
	double info_gain_delta = 0;
	vector<line> lines_subset;
	//for each line
	for (i = 0; i < lines.size(); i++) {
		//if the specific attribute of that line is less than the threshold
		if (lines[i].attribute_values[index] < threshold) {
			//add it to the subset
			lines_subset.push_back(lines[i]);
		}
	}
	//compute the first part of the info gain
	info_gain_delta -= ((double)lines_subset.size() / lines.size()) * get_entropy(lines_subset, POSITIVECLASSIFICATION);
	//clear the subset
	lines_subset.clear();
	//for each line
	for (i = 0; i < lines.size(); i++) {
		//if the specific attribute is greater than or equal to the threshold
		if (lines[i].attribute_values[index] >= threshold) {
			//add it to the subset
			lines_subset.push_back(lines[i]);
		}
	}
	//compute the second part of the info gain
	info_gain_delta -= ((double)lines_subset.size() / lines.size()) * get_entropy(lines_subset, POSITIVECLASSIFICATION);
	return info_gain_delta;
}

//Computes log base 2 of a number
double log_2(double dbl) {
	//include this in case the function gets called with a 0
	if (dbl)
		return log(dbl) / log(2);
	return 0;
}
//Function gets attribute values and stores them in a vector
vector<double> get_attribute_values(vector<line> lines, int index, double *sum) {
	vector<double> values;
	unsigned int i;
	int _sum = 0;
	//for each line
	for (i = 0; i < lines.size(); i++) {
		//add  the specific attribute value
		_sum += lines[i].attribute_values[index];
		if (find(values.begin(), values.end(), lines[i].attribute_values[index]) != values.end())
			continue;
		//enter the specific attribute of the current line into values
		values.push_back(lines[i].attribute_values[index]);
	}
	if (sum != NULL)
		*sum = _sum;
	return values;
}
//Use our other functions to make correct splits and create the tree
node *build_tree(node *root_node, const vector<line>& lines, vector<int> attributes, string POSITIVECLASSIFICATION) {
	unsigned int i, _index;
	double _avg, avg, gain, _gain, entropy = get_entropy(lines, POSITIVECLASSIFICATION);

	root_node->dataset_size = lines.size();
	//If our entropy is 0 then the data of the node is just the classification
	if (entropy == 0) {
		root_node->data = lines[0].classification;
		return root_node;
	}

	get_attribute_values(lines, attributes[0], &avg);
	//find the average by dividing by the number of elements
	avg /= lines.size();
	//entropy gain
	gain = entropy + compute_info_gain_entropy_delta(lines, avg, attributes[0], POSITIVECLASSIFICATION);
	_index = 0;
	_gain = gain;
	_avg = avg;
	//for each attribute
	for (i = 1; i < attributes.size(); i++) {
		//collect the values
		get_attribute_values(lines, attributes[i], &avg);
		avg /= lines.size();
		gain = entropy + compute_info_gain_entropy_delta(lines, avg, attributes[i], POSITIVECLASSIFICATION);
		//Sample Data initially evenly divided resulting in
		if (gain < _gain) {
			_index = i;
			_gain = gain;
			_avg = avg;
		}
	}
	//build the child nodes
	root_node->lnode = new node();
	root_node->rnode = new node();
	root_node->threshold = _avg;
	root_node->attribute_index = attributes[_index];

	vector<int> _attributes = attributes;
	//erasing the attribute we used
	_attributes.erase(_attributes.begin() + _index);

	vector<line> lines_subset;
	//for each line
	for (i = 0; i < lines.size(); i++) {
		//if the specific attribute in the line is less than the average add the line to the subset
		if (lines[i].attribute_values[attributes[_index]] < _avg) {
			lines_subset.push_back(lines[i]);
		}
	}
	//build a tree off the left child of the current node
	build_tree(root_node->lnode, lines_subset, _attributes, POSITIVECLASSIFICATION);
	//clear the subset
	lines_subset.clear();
	//if the specific attribute in the line is less than the average add the line to the subset
	for (i = 0; i < lines.size(); i++) {
		if (lines[i].attribute_values[attributes[_index]] >= _avg) {
			lines_subset.push_back(lines[i]);
		}
	}
	//build a tree off the right child of the current node
	build_tree(root_node->rnode, lines_subset, _attributes, POSITIVECLASSIFICATION);

	return root_node;
}
//to output entire tree
void output_tree(node *root_node) {
	int i, depth = get_max_depth(root_node);
	//for each level, output the nodes
	for (i = 0; i < depth; i++) {
		output_nodes_at_depth(root_node, i, TERMINAL_MIN_WIDTH);
		cout << endl;
		cout << endl;
	}
}

//output nodes at each level of the tree
void output_nodes_at_depth(node *root_node, int depth, int width) {
	stringstream buff;
	int space;
	//if at 0 depth
	if (depth == 0) {
		//if the pointer points to nothing output a group of spaces
		if (root_node == NULL)
			cout << setw(width) << "";
		else {
			//if the attribute index is not -1
			if (root_node->attribute_index != -1)
				//add this string to the display buffer
				buff << "[attr " << root_node->attribute_index << "] " << root_node->threshold;
			else
				//add these strings to the display buffer
				buff << root_node->data;
			buff << " [" << root_node->dataset_size << "]";
			space = width - buff.str().length();
			cout << setw(space / 2) << "" << buff.str() << setw(space / 2) << "";
		}
		return;
	}
	//recurse down the tree until it hits a NULL pointer for the root
	if (root_node != NULL) {
		output_nodes_at_depth(root_node->lnode, depth - 1, width / 2);
		output_nodes_at_depth(root_node->rnode, depth - 1, width / 2);
	}
	else {
		output_nodes_at_depth(NULL, depth - 1, width / 2);
		output_nodes_at_depth(NULL, depth - 1, width / 2);
	}
}
//recursively find total amount of levels of the tree
int get_max_depth(node *root_node) {
	if (root_node == NULL)
		return 0;
	int rlen = get_max_depth(root_node->rnode);
	int llen = get_max_depth(root_node->lnode);

	return 1 + (llen > rlen ? llen : rlen);
}
