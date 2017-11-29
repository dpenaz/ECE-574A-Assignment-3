#include <string>
#include <vector>
#include <queue>
#include "Node.h"

using namespace std;

Node::Node(string Op)
{
	this->nodeOp = Op;
	this->doneFlag = false;			// Operation has not been performed
	if (!Op.compare("*"))			// Set Cycle Delay depending on Operation
		this->cycleDelay = 2;
	else if (!Op.compare("/") || !Op.compare("%"))
		this->cycleDelay = 3;
	else
		this->cycleDelay = 1;
}

Node::~Node() {}

void Node::setOutPut(string val)
{
	this->outPut = val;
}

void Node::addInputs(string val)
{
	this->inPuts.push_back(val);
}

void Node::addParent(Node* p)
{
	this->parents.push_back(p);
}

int Node::getParentsSize()
{
	return this->parents.size();
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

void Node::setStartTime(int t)
{
	this->startTime = t;
}

int Node::getStartTime()
{
	return this->startTime;
}

void Node::complete()
{
	this->doneFlag = true;
}

string Node::getVerilogCode()
{
	// Needs to be implimented
	return this->outPut;
}

string Node::getOutPut()
{
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