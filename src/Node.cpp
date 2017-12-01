#include <string>
#include <vector>
#include <queue>
#include "Node.h"

using namespace std;

Node::Node(string Op)
{
	this->nodeOp = Op;

	/*Initialize ASAP & ALAP*/
	this->ASAP_start = 0;	
	this->ALAP_start = 0;

	if (!Op.compare("*"))			// Set Cycle Delay depending on Operation
		this->latency = 2;
	else if (!Op.compare("/") || !Op.compare("%"))
		this->latency = 3;
	else
		this->latency = 1;
}

Node::~Node() {}

void Node::setWidth()
{
	this->width = ALAP_start - ASAP_start + 1;
	this->prob_val = 1.0 / this->width;
}

void Node::addInputs(string val)
{
	this->inPuts.push_back(val);
}

void Node::addParent(Node* p)
{
	this->parents.push_back(p);
}

void Node::removeParent(Node* p)
{
	for (int i = 0; i < this->parents.size(); ++i)
		if (this->parents[i] == p)
		{
			this->parents.erase(parents.begin() + i);
			return;
		}
}

void Node::addChild(Node* c)
{
	this->children.push_back(c);
}

string Node::getVerilogCode()
{
	// Needs to be implimented
	return this->outPut;
}

bool Node::chkIfParent(string var)
{
	auto it = find(this->inPuts.begin(), this->inPuts.end(), var);
	if (it != this->inPuts.end())
		return true;
	return false;
}

bool Node::findChild(string var)
{
	if (!var.compare(this->outPut))
		return true;
	return false;
}