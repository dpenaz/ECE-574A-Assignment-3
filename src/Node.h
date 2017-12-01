#include <string>
#include <vector>
#include <tuple>
#include <list>

using namespace std;

class Node
{
public:
	/******************
	**** Variables ****
	******************/
	bool visitedFlag;		// Lets child node know if node has been completed

	int latency;			// Cycles to execute
	int ALAP_start;
	int ASAP_start;
	int width;

	string outPut;
	string nodeOp;
	
	list<string> inPuts;
	
	double prob_val;
	int finalScheduleTime;			// When the Operation should occure
	vector<tuple<int, double>> forces;	// Used to determine finalScheduleTime ::  tuple<"cycle", "TotalForce">
	
	vector<Node*> parents;	// Nodes that happen before
	vector<Node*> children;	// Nodes that are dependent on this node

	/******************
	**** Functions ****
	******************/

	Node(string Op);
	~Node();

	void addInputs(string val);
	void addParent(Node* p);
	void removeParent(Node* p);
	void addChild(Node* c);
	void setWidth();

	string getVerilogCode();
	bool findChild(string var);
	bool chkIfParent(string var);
};