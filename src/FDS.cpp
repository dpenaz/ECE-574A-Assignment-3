#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <iomanip>
#include <tuple>
#include "FDS.h"

using namespace std;

int numStates(vector<Node*> myNodes)
{
	return myNodes[myNodes.size() - 1]->finalScheduleTime;
}

string stateCode(vector<Node*> myNodes)
{
	ostringstream oss;
	const int nStates = numStates(myNodes);
	for ( int i = 1; i <= nStates; ++i)
	{
		vector<tuple<bool, string>> conds;
		oss << "        " << i + 2 << ": begin" << endl;

		for ( int j = 0; j < myNodes.size(); ++j) {
			if (myNodes[j]->finalScheduleTime == i) {


				for (auto it = myNodes[j]->conditions.begin(); it != myNodes[j]->conditions.end(); ++it) {
					if (conds.size() == 0) {
						conds.push_back(*it);
						oss << "            if (" << get<1>(*it) << ") begin" << endl;
					} else {
						if (conds.size() >= 2 && !get<1>(conds[conds.size()-2]).compare(get<1>(*it))) {
							oss << "            end" << endl;
							conds.pop_back();
						} else if (get<1>(conds.back()).compare(get<1>(*it))) {
							conds.push_back(*it);
							oss << "            if (" << get<1>(*it) << ") begin" << endl;
						} else {
							if (get<0>(conds.back()) != get<0>(*it)) {
								oss << "            end else begin" << endl;
								conds.pop_back();
								conds.push_back(*it);
							}
						}
					}
				}


				string op = myNodes[j]->nodeOp;
				list<string> ins = myNodes[j]->inPuts;
				oss << "             " << myNodes[j]->outPut << " = ";
				if (!op.compare("=")) {
					oss << ins.front() << ";" << endl;
				}
				else if (op.compare("?")) {
					oss << ins.front() << " " << op << " " << ins.back() << ";" << endl;
				}
				else {
					string in1 = ins.front();
					ins.pop_front();
					string in2 = ins.front();
					ins.pop_front();
					string in3 = ins.front();
					oss << in1 << " " << op << " " << in2 << " : " << in3 << ";" << endl;
				}
			}
		}
		
		for (auto it = conds.begin(); it != conds.end(); ++it)
			oss << "            end" << endl;

		if (i < nStates)
			oss << "             State <= State + 1;" << endl;
		else
			oss << "             State <= 1;" << endl;

		oss << "        end" << endl;
	}
	return oss.str();
}

void printNodes(vector<Node*> myNodes)
{
	for ( int i = 0; i < myNodes.size(); ++i)
	{
		cout << "Node #" << i << endl;
		cout << "Num of Parents: " << myNodes[i]->parents.size() << endl;
		cout << "Num of Children: " << myNodes[i]->children.size() << endl;
		cout << "Output: " << myNodes[i]->getVerilogCode() << endl;
		cout << "Operation: " << myNodes[i]->nodeOp << endl;
		cout << "ASAP: " << myNodes[i]->ASAP_start << endl;
		cout << "ALAP: " << myNodes[i]->ALAP_start << endl;
		cout << "Width: " << myNodes[i]->width << endl;
		cout << "Prob Val: " << myNodes[i]->prob_val << endl;
		cout << "FINAL SCHEDULE: " << myNodes[i]->finalScheduleTime << endl << endl;
	}
}

void printDistribution()
{
	for ( int i = 0; i <= 3; ++i)
	{
		switch (i)
		{
		case multi: cout << "Multi" << endl; break;
		case add_sub: cout << "Add/Sub" << endl; break;
		case div_mod: cout << "Div/Mod" << endl; break;
		case logic: cout << "Logic" << endl; break;
		default: break;
		}

		for ( int col = 0; col <= 1; ++col)
		{
			cout << "\t";
			for ( int row = 1; row < op_Prob[i].size(); ++row)
			{
				if (col == 0)
					cout << row << "\t";
				else
					cout << setprecision(2) << op_Prob[i][row] << "\t";
			}
			cout << endl;
		}
		cout << endl << endl;
	}
}

void resetFlag(vector<Node*> myNodes)
{
	for ( int i = 0; i < myNodes.size(); ++i)
	{
		myNodes[i]->visitedFlag = false;
	}
}

void connectNodes(vector<Node*> myNodes)
{
	for ( int i = 0; i < myNodes.size(); ++i)
	{
		for ( int j = i; j < myNodes.size(); ++j)
		{
			if (myNodes[j]->chkIfParent(myNodes[i]->outPut))
			{
				myNodes[j]->addParent(myNodes[i]);
				myNodes[i]->addChild(myNodes[j]);
			}
		}
	}
}

void cal_ALAP(vector<Node*> myNodes, int cycles)
{
	int time, temp;
	bool done = false;
	bool ready;
	while (!done)
	{
		done = true;
		for ( int i = 0; i < myNodes.size(); ++i)
		{
			ready = true;
			if (myNodes[i]->children.size() == 0) // bottom of tree
			{
				time = cycles - myNodes[i]->latency + 1;
				myNodes[i]->ALAP_start = time;
			}
			else
			{
				time = 100000;
				temp = 0;
				for ( int j = 0; j < myNodes[i]->children.size(); ++j)	//check children to see if scheduled
				{
					if (myNodes[i]->children[j]->ALAP_start == 0)
					{
						done = false;
						ready = false;
						break;
					}
					temp = myNodes[i]->children[j]->ALAP_start - myNodes[i]->latency;
					if (temp < time)		// get Largest wait time from children
						time = temp;
				}
				if (ready) {
					myNodes[i]->ALAP_start = time;
				}
			}
		}
	}
}

int cal_ASAP(vector<Node*> myNodes)
{
	int max = 0;
	int time, temp;
	bool done = false;
	while (!done)
	{
		done = true;
		for ( int i = 0; i < myNodes.size(); ++i)
		{
			if (myNodes[i]->parents.size() == 0) // Top of tree
				myNodes[i]->ASAP_start = 1;
			else
			{
				time = 0;
				temp = 0;
				for ( int j = 0; j < myNodes[i]->parents.size(); ++j)	//check parents to see if scheduled
				{
					if (myNodes[i]->parents[j]->ASAP_start == 0)
					{
						done = false;
						break;
					}
					temp = myNodes[i]->parents[j]->ASAP_start + myNodes[i]->parents[j]->latency;
					if (temp > time)		// get Largest wait time from parents
						time = temp;
				}
				if (done) {
					myNodes[i]->ASAP_start = time;
					if (time + myNodes[i]->latency > max)
						max = time + myNodes[i]->latency;
				}
			}
		}
	}
	return max;
}

void cal_width(vector<Node*> myNodes)
{
	for ( int i = 0; i < myNodes.size(); ++i)
	{
		myNodes[i]->setWidth();
	}
}

int vectNum(string s)
{
	if (!s.compare("*"))	// multiplier resource
	{
		return multi;
	}
	else if (!s.compare("+") || !s.compare("-"))	// adder/subtractor resource
	{
		return add_sub;	//add_subDist
	}
	else if (!s.compare("/") || !s.compare("%"))	// divider/modulo resource
	{
		return div_mod; //div_modDist
	}
	else	// logic/logical resource
	{
		return logic;	//logicDist
	}
}

void resetTypeDistVectors(vector<Node*> myNodes)
{
	for (int i = 0; i <= 3; ++i)
	{
		for (int j = 0; j < op_Prob[i].size(); ++j)
		{
			op_Prob[i][j] = 0.0;
		}
	}
	cal_TypeDistribution(myNodes);
}

void cal_TypeDistribution(vector<Node*> myNodes)
{
	for ( int i = 0; i < myNodes.size(); ++i)
	{
		for ( int j = myNodes[i]->ASAP_start; j <= myNodes[i]->ALAP_start; ++j)
		{
			if (!myNodes[i]->nodeOp.compare("*"))	// multiplier resource
				op_Prob[multi][j] += myNodes[i]->prob_val;
			else if (!myNodes[i]->nodeOp.compare("+") || !myNodes[i]->nodeOp.compare("-"))	// adder/subtractor resource
				op_Prob[add_sub][j] += myNodes[i]->prob_val;
			else if (!myNodes[i]->nodeOp.compare("/") || !myNodes[i]->nodeOp.compare("%"))	// divider/modulo resource
				op_Prob[div_mod][j] += myNodes[i]->prob_val;
			else	// logic/logical resource
				op_Prob[logic][j] += myNodes[i]->prob_val;
		}
	}
}

void cal_ForceDir(vector<Node*> myNodes)
{
	for ( int i = 0; i < myNodes.size(); ++i)
	{
		myNodes[i]->finalScheduleTime = get<0>(forceDir(myNodes[i], 0, true, true));	// send in zero to begin (no interference for node)
		myNodes[i]->ALAP_start = myNodes[i]->finalScheduleTime;
		myNodes[i]->ASAP_start = myNodes[i]->finalScheduleTime;
		myNodes[i]->prob_val = 1.0;
		//		printDistribution();
		resetTypeDistVectors(myNodes);
		//		printDistribution();
	}
}

tuple<int, double> forceDir(Node* node, int cycleNum, bool first, bool Successor)
{
	node->visitedFlag = true;
	if ((cycleNum < node->ASAP_start || node->ALAP_start < cycleNum) && !first)	// check to see if schedule of last node affects the current node.
		return { 0,0 };

	tuple<int, double> tempForce(-1, 0.0);
	tuple<int, double> forceSum(-1, 10000.0);

	double prob;
	int timing = 0;

	int probVec = vectNum(node->nodeOp);

	for ( int cycle = node->ASAP_start; cycle <= node->ALAP_start; ++cycle)
	{
		if (first)
			resetFlag(myNodes);
		for ( int k = 0; k < op_Prob[probVec].size(); ++k)
		{
			if (!first)
			{
				if (Successor && (k <= cycleNum))
				{
					prob = 0.0;
				}
				else if (!Successor && (k >= cycleNum))
				{
					prob = 0.0;
				}
				else if (cycle == k)
					prob = 1;
				else
					prob = 0.0;
			}
			else if (cycle == k)
				prob = 1;
			else
				prob = 0.0;
			get<1>(tempForce) += op_Prob[probVec][k] * (prob - node->prob_val);
		}

		for ( int i = 0; i < node->parents.size(); ++i)		// Calculate force for PREDECESSORs
		{
			if (!node->parents[i]->visitedFlag)
				get<1>(tempForce) += get<1>(forceDir(node->parents[i], cycle, false, false));
		}
		for ( int i = 0; i < node->children.size(); ++i)	// Calculate force for SUCCESSORs
		{
			if (!node->children[i]->visitedFlag)
				get<1>(tempForce) += get<1>(forceDir(node->children[i], cycle, false, true));
		}
		if (get<1>(tempForce) < get<1>(forceSum))
		{
			get<1>(forceSum) = get<1>(tempForce);
			if (first)
				get<0>(forceSum) = cycle;
		}
	}
	return forceSum;
}