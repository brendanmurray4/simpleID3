#ifndef HEADER_FILE_H_INCLUDED

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class line {
public:
	vector<double> attribute_values;
	string classification;
};

class node {
public:
	node() { attribute_index = -1; lnode = NULL; rnode = NULL; }
	int attribute_index;
	int dataset_size;
	double threshold;
	string data;
	node *lnode;
	node *rnode;
};

vector<string> split(const string& str, const char& delim);
double get_entropy(vector<line> lines, string POSITIVECLASSIFICATION);
double log_2(double dbl);
vector<double> get_attribute_values(vector<line> lines, int index, double *sum);
double compute_info_gain_entropy_delta(vector<line> lines, double threshold, int index, string POSITIVECLASSIFICATION);
node *build_tree(node *root_node, const vector<line>& lines, vector<int> attributes, string POSITIVECLASSIFICATION);
void output_tree(node *root_node);
void output_nodes_at_depth(node *root_node, int depth, int width);
int get_max_depth(node *root_node);

#endif
