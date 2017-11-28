#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include "Verilog_Imp.h"

using namespace std;

bool checkKey(string key, const map<string, vector<string> > &my_map);
extern ostringstream oss;
extern vector<tuple<string, vector<string>,string, bool>> graph;

int numGraphElems = 0;

bool is_X(string sym, const map<string, vector<string> > &my_map, string type)
{
	map<string, vector<string> >::const_iterator it = my_map.find(sym);
	if (it == my_map.end()) {
		cerr << "Error: '" << sym << "' is not a variable";
		return false;
	}
	string symFunc = it->second[0];
	return !symFunc.compare(type);
}

unsigned int numInc = 0, numDec = 0, numAdd = 0, numSub = 0, numMod = 0, numDiv = 0, numShl = 0, numShr = 0, numComp = 0, numMul = 0, numMux = 0, numReg = 0;

bool assign_op_result(string op, string line, const map<string, vector<string> > &my_map)
{
	string out, in1, in2;
	istringstream iss(line);

	string curSym;

	// OUT
	iss >> curSym;

	if (!checkKey(curSym, my_map)) {
		cerr << "Error: unexpected symbol: " << curSym << " in following line:" << endl << line << endl;
		return false;
	}
	else if (!is_X(curSym, my_map, "register") && !is_X(curSym, my_map, "output")) {
		cerr << "Error: '" << curSym << "' is not a variable (register) or output in following line:" << endl << line << endl;
		return false;
	}

	out = curSym;
	// =
	iss >> curSym;

	if (curSym.compare("=")) {
		cerr << "Expected '=' instead of '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}

	// IN1
	iss >> curSym;

	//bool regout1 = false;
	if (!checkKey(curSym + "_out", my_map)) {
		if (!checkKey(curSym, my_map)) {
			cerr << "Error: unexpected symbol: " << curSym << " in following line:" << endl << line << endl;
			return false;
		}
		else if (!is_X(curSym, my_map,"input") && !is_X(curSym, my_map, "register")) {
			cerr << "Error: " << curSym << " is not an input or a variable (register) in following line:" << endl << line << endl;
			return false;
		}
	}
	else {
		curSym = curSym + "_out";
	//	regout1 = true;
	}
	in1 = curSym;

	// operation
	iss >> curSym;

	if (curSym.compare(op)) {
		cerr << "Error: expected '" << op << "', not '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}

	// IN2
	iss >> curSym;

	//bool regout2 = false;
	if (!checkKey(curSym + "_out", my_map)) {
		if (curSym.compare("1")) {
			if (!checkKey(curSym, my_map)) {
				cerr << "Error: unexpected symbol: " << curSym << " in following line:" << endl << line << endl;
				return false;
			}
			else if (!is_X(curSym, my_map, "input") && !is_X(curSym, my_map,"register")) {
				cerr << "Error: " << curSym << " is not an input or a variable (register) in following line:" << endl << line << endl;
				return false;
			}
		}
	}
	else {
		curSym = curSym + "_out";
	//	regout2 = true;
	}
	in2 = curSym;

	// end of line
	iss >> curSym;
	if (!iss.eof()) {
		cerr << "Error: expected end of line, not '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}


	string opstr;

	if (!op.compare("+")) {
		opstr = "+";
	}
	else if (!op.compare("-")) {
		opstr = "-";
	}
	else if (!op.compare("%")) {
		opstr = "%";
	}
	else if (!op.compare("/")) {
		opstr = "/";
	}
	else if (!op.compare("<<")) {
		opstr = "<<";
	}
	else if (!op.compare(">>")) {
		opstr = ">>";
	}
	else if (!op.compare("==")) {
		opstr = "==";
	}
	else if (!op.compare("<")) {
		opstr = "<";
	}
	else if (!op.compare(">")) {
		opstr = ">";
	}
	else if (!op.compare("*")) {
		opstr = "*";
	}
	else {
		cerr << "Unknown operation: " << op << "in line: " << endl << line << endl;
		return false;
	}

	oss << out + " " + opstr + " "  + in1 + " " + in2 << endl;
	
	return true;
}

bool REG_(string line, const map<string, vector<string> > &my_map)
{
	string out, in;
	istringstream iss(line);

	string curSym;

	// OUT
	iss >> curSym;
	


	if (!checkKey(curSym, my_map)) {
		cerr << "Error: unexpected symbol '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}
	if (!is_X(curSym, my_map,"output")) {
		cerr << "Error: '" << curSym << "' is not an output or register in following line:" << endl << line << endl;
		return false;
	}

	out = curSym;

	// =
	iss >> curSym;

	if (curSym.compare("=")) {
		cerr << "Expected '=' instead of '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}

	// IN1
	iss >> curSym;

	if (!checkKey(curSym, my_map)) {
		cerr << "Error: unexpected symbol: " << curSym << " in following line:" << endl << line << endl;
		return false;
	}
	else if (!is_X(curSym, my_map,"input") && !is_X(curSym, my_map,"register")) {
		cerr << "Error: " << curSym << " is not an input or a variable (register) in following line:" << endl << line << endl;
		return false;
	}
	in = curSym;

	// end of line
	iss >> curSym;
	if (!iss.eof()) {
		cerr << "Error: expected end of line, not '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}

	oss << out + " = " + in << endl;

	vector<string> v{ in };
	graph.push_back(make_tuple("REG", v, out, false));

	return true;
};

bool MUX2x1_(string line, const map<string, vector<string> > &my_map)
{
	string out, in1, in2, in3;
	istringstream iss(line);

	string curSym;

	// OUT
	iss >> curSym;

	if (!checkKey(curSym, my_map)) {
		cerr << "Error: unexpected symbol: " << curSym << " in following line:" << endl << line << endl;
		return false;
	}
	else if (!is_X(curSym, my_map,"register") && !is_X(curSym, my_map,"output")) { //TBD
		cerr << "Error: '" << curSym << "' is not a variable (register) or output in following line:" << endl << line << endl;
		return false;
	}

	out = curSym;

	// =
	iss >> curSym;

	if (curSym.compare("=")) {
		cerr << "Expected '=' instead of '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}

	// IN1
	iss >> curSym;

	if (!checkKey(curSym, my_map)) {
		cerr << "Error: unexpected symbol: " << curSym << " in following line:" << endl << line << endl;
		return false;
	}
	else if (!is_X(curSym, my_map,"input") && !is_X(curSym, my_map,"register")) {
		cerr << "Error: " << curSym << " is not an input or a variable (register) in following line:" << endl << line << endl;
		return false;
	}
	in1 = curSym;

	// operation
	iss >> curSym;

	if (curSym.compare("?")) {
		cerr << "Error: expected '?', not '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}

	// IN2
	iss >> curSym;

	if (!checkKey(curSym, my_map)) {
		cerr << "Error: unexpected symbol: " << curSym << " in following line:" << endl << line << endl;
		return false;
	}
	else if (!is_X(curSym, my_map,"input") && !is_X(curSym, my_map,"register")) {
		cerr << "Error: " << curSym << " is not an input or a variable (register) in following line:" << endl << line << endl;
		return false;
	}
	in2 = curSym;

	// operation
	iss >> curSym;

	if (curSym.compare(":")) {
		cerr << "Error: expected ':', not '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}

	// IN3
	iss >> curSym;

	if (!checkKey(curSym, my_map)) {
		cerr << "Error: unexpected symbol: " << curSym << " in following line:" << endl << line << endl;
		return false;
	}
	else if (!is_X(curSym, my_map,"input") && !is_X(curSym, my_map,"register")) {
		cerr << "Error: " << curSym << " is not an input or a variable (register) in following line:" << endl << line << endl;
		return false;
	}
	in3 = curSym;

	// end of line
	iss >> curSym;
	if (!iss.eof()) {
		cerr << "Error: expected end of line, not '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}

	vector<string> v{ in3,in2,in1 };
	graph.push_back(make_tuple("MUX2x1" , v,out, false ));
	oss << out + " = " + in1 + " ? " + in2 + " : " + in3 << endl;

	return true;
}

