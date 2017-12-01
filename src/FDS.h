#include "Node.h"

using namespace std;

void printNodes(vector<Node*> myNodes);

void connectNodes(vector<Node*> myNodes);

void cal_ALAP(vector<Node*> myNodes, int cycles);

void cal_ASAP(vector<Node*> myNodes);

void cal_width(vector<Node*> myNodes);

void cal_TypeDistribution(vector<Node*> myNodes, vector<double> &mult, vector<double> &add_sub, 
	                      vector<double> &logic, vector<double> &div_mod);

void cal_ForceDir(vector<Node*> myNodes, vector<double> &mult, vector<double> &add_sub,
	              vector<double> &logic, vector<double> &div_mod);

void printDistribution(vector<double> v);