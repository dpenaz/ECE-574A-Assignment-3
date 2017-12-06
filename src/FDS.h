#include "Node.h"

using namespace std;

/****************************
****  Global variables  ****
****************************/

extern vector<vector<double>> op_Prob;

/****************************/

enum Probability {
	multi,
	add_sub,
	div_mod,
	logic
};

/********************************
****  Function Delcarations  ****
********************************/

int vectNum(string s);

int numStates(vector<Node*> myNodes);

string stateCode(vector<Node*> myNodes);

void printNodes(vector<Node*> myNodes);

void connectNodes(vector<Node*> myNodes);

void cal_ALAP(vector<Node*> myNodes, int cycles);

void cal_ASAP(vector<Node*> myNodes);

void cal_width(vector<Node*> myNodes);

void cal_TypeDistribution(vector<Node*> myNodes);

void cal_ForceDir(vector<Node*> myNodes);

tuple<int, double> forceDir(Node* node, int cycleNum, bool first, bool Successor);

void printDistribution();