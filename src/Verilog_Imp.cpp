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
	string prefix = "";
	istringstream iss(line);
	int bitSize;

	string curSym;

	// OUT
	iss >> curSym;

	if (!checkKey(curSym, my_map)) {
		cerr << "Error: unexpected symbol: " << curSym << " in following line:" << endl << line << endl;
		return false;
	}
	else if (!is_X(curSym, my_map, "wire")) {
		cerr << "Error: '" << curSym << "' is not a wire in following line:" << endl << line << endl;
		return false;
	}

	out = curSym;
	const string out_orig = out;
	bitSize = getBitSize(out, my_map);
	if (signedVar(out, my_map))	//determine signed or unsigned
		prefix = "S";
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
		else if (!is_X(curSym, my_map,"input") && !is_X(curSym, my_map, "wire")) {
			cerr << "Error: " << curSym << " is not an input or a wire in following line:" << endl << line << endl;
			return false;
		}
	}
	else {
		curSym = curSym + "_out";
	//	regout1 = true;
	}
	in1 = curSym;
	const string in1_orig = in1;

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
			else if (!is_X(curSym, my_map, "input") && !is_X(curSym, my_map,"wire")) {
				cerr << "Error: " << curSym << " is not an input or a wire in following line:" << endl << line << endl;
				return false;
			}
		}
	}
	else {
		curSym = curSym + "_out";
	//	regout2 = true;
	}
	in2 = curSym;
	const string in2_orig = in2;

	// end of line
	iss >> curSym;
	if (!iss.eof()) {
		cerr << "Error: expected end of line, not '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}

	bitManip(in1, my_map);
	bitManip(in2, my_map);

	string outstr, opstr;
	bool gt = false, lt = false, eq = false;
	bool add = false, sub = false;

	if (!op.compare("+")) {
		add = true;
		if (!in2.compare("1")) {
			opstr = "INC";
			outstr = prefix + "INC#(" + to_string(bitSize) + ") Inc_" + to_string(++numInc);
		}
		else {
			opstr = "ADD";
			outstr = prefix + "ADD#(" + to_string(bitSize) + ") Adder_" + to_string(++numAdd);
		}
	}
	else if (!op.compare("-")) {
		sub = true;
		if (!in2.compare("1")) {
			opstr = "DEC";
			outstr = prefix + "DEC#(" + to_string(bitSize) + ") Dec_" + to_string(++numDec);
		}
		else {
			opstr = "SUB";
			outstr = prefix + "SUB#(" + to_string(bitSize) + ") Sub_" + to_string(++numSub);
		}
	}
	else if (!op.compare("%")) {
		opstr = "MOD";
		outstr = prefix + "MOD#(" + to_string(bitSize) + ") Mod_" + to_string(++numMod);
	}
	else if (!op.compare("/")) {
		opstr = "DIV";
		outstr = prefix + "DIV#(" + to_string(bitSize) + ") Div_" + to_string(++numDiv);
	}
	else if (!op.compare("<<")) {
		opstr = "SHL";
		outstr = prefix + "SHL#(" + to_string(bitSize) + ") Shl_" + to_string(++numShl);
	}
	else if (!op.compare(">>")) {
		opstr = "SHR";
		outstr = prefix + "SHR#(" + to_string(bitSize) + ") Shr_" + to_string(++numShr);
	}
	else if (!op.compare("==")) {
		eq = true;
		bitSize = getLgBit(line, my_map);
		opstr = "COMP";
		outstr = prefix + "COMP#(" + to_string(bitSize) + ") Comp_" + to_string(++numComp);
	}
	else if (!op.compare("<")) {
		lt = true;
		bitSize = getLgBit(line, my_map);
		opstr = "COMP";
		outstr = prefix + "COMP#(" + to_string(bitSize) + ") Comp_" + to_string(++numComp);
	}
	else if (!op.compare(">")) {
		gt = true;
		bitSize = getLgBit(line, my_map);
		opstr = "COMP";
		outstr = prefix + "COMP#(" + to_string(bitSize) + ") Comp_" + to_string(++numComp);
	}
	else if (!op.compare("*")) {
		opstr = "MUL";
		outstr = prefix + "MUL#(" + to_string(bitSize) + ") Mul_" + to_string(++numMul);
	}
	else {
		cerr << "Unknown operation: " << op << "in line: " << endl << line << endl;
		return false;
	}

	if ((add || sub) && !in2.compare("1")) {
		vector<string> v{ in1_orig };
		graph.push_back(make_tuple(opstr + to_string(bitSize), v, out_orig, false ));
	}
	else {
		vector<string> v{ in1_orig,in2_orig };
		graph.push_back(make_tuple(opstr + to_string(bitSize), v, out, false ));
	}

	if (gt)
		outstr = outstr + "(" + in1 + "," + in2 + "," + out + ",,);";
	else if (lt)
		outstr = outstr + "(" + in1 + "," + in2 + ",," + out + ",);";
	else if (eq)
		outstr = outstr + "(" + in1 + "," + in2 + ",,," + out + ");";
	else if (!in2.compare("1"))
		outstr = outstr + "(" + in1 + "," + out + ");";
	else
		outstr = outstr + "(" + in1 + "," + in2 + "," + out + ");";

	oss << outstr << endl;
	
	return true;
}

bool Add_or_INC(string line, const map<string, vector<string> > &my_map)
{
	return assign_op_result("+", line, my_map);
};

bool SUB_or_DEC(string line, const map<string, vector<string> > &my_map)
{
	return assign_op_result("-", line, my_map);
};

bool MOD_(string line, const map<string, vector<string> > &my_map)
{
	return assign_op_result("%", line, my_map);
};

bool DIV_(string line, const map<string, vector<string> > &my_map)
{
	return assign_op_result("/", line, my_map);
};

bool SHL_(string line, const map<string, vector<string> > &my_map)
{
	return assign_op_result("<<", line, my_map);
};

bool SHR_(string line, const map<string, vector<string> > &my_map)
{
	return assign_op_result(">>", line, my_map);
};

bool COMP_eq(string line, const map<string, vector<string> > &my_map)
{
	return assign_op_result("==", line, my_map);
};

bool COMP_lt(string line, const map<string, vector<string> > &my_map)
{
	return assign_op_result("<", line, my_map);
};

bool COMP_gt(string line, const map<string, vector<string> > &my_map)
{
	return assign_op_result(">", line, my_map);
};

bool MUL_(string line, const map<string, vector<string> > &my_map)
{
	return assign_op_result("*", line, my_map);
};

bool REG_(string line, const map<string, vector<string> > &my_map)
{
	string out, in;
	string prefix = "";
	istringstream iss(line);
	int bitSize;

	string curSym;

	// OUT
	iss >> curSym;
	

	bool isreg = false;

	if (is_X(curSym, my_map,"register")) {
		isreg = true;
	}
	else {
		if (!checkKey(curSym, my_map)) {
			cerr << "Error: unexpected symbol '" << curSym << "' in following line:" << endl << line << endl;
			return false;
		}
		if (!is_X(curSym, my_map,"output")) {
			cerr << "Error: '" << curSym << "' is not an output or register in following line:" << endl << line << endl;
			return false;
		}
	}

	out = curSym;
	const string out_orig = out;
	bitSize = getBitSize(out, my_map);
	if (signedVar(out, my_map))	//determine signed or unsigned
		prefix = "S";

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
	else if (!is_X(curSym, my_map,"input") && !is_X(curSym, my_map,"wire")) {
		cerr << "Error: " << curSym << " is not an input or a wire in following line:" << endl << line << endl;
		return false;
	}
	in = curSym;
	const string in_orig = in;

	// end of line
	iss >> curSym;
	if (!iss.eof()) {
		cerr << "Error: expected end of line, not '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}
	bitManip(in, my_map);
	if (isreg)
		oss << prefix + "REG#(" + to_string(bitSize) + ") " << out << "(" << in << "," << out << "_out,clk,rst);" << endl;
	else
		oss << prefix + "REG#(" + to_string(bitSize) + ") Reg_" << ++numReg << "(" << in << "," << out << ",clk,rst);" << endl;

	vector<string> v{ in_orig };
	graph.push_back(make_tuple("REG" + to_string(bitSize), v, out_orig, false));

	return true;
};

bool MUX2x1_(string line, const map<string, vector<string> > &my_map)
{
	string out, in1, in2, in3;
	string prefix = "";
	istringstream iss(line);
	int bitSize;

	string curSym;

	// OUT
	iss >> curSym;

	if (!checkKey(curSym, my_map)) {
		cerr << "Error: unexpected symbol: " << curSym << " in following line:" << endl << line << endl;
		return false;
	}
	else if (!is_X(curSym, my_map,"wire") && !is_X(curSym, my_map,"output")) { //TBD
		cerr << "Error: '" << curSym << "' is not a wire or output in following line:" << endl << line << endl;
		return false;
	}

	out = curSym;
	const string out_orig = out;
	bitSize = getBitSize(out, my_map);
	if (signedVar(out, my_map))	//determine signed or unsigned
		prefix = "S";

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
	else if (!is_X(curSym, my_map,"input") && !is_X(curSym, my_map,"wire")) {
		cerr << "Error: " << curSym << " is not an input or a wire in following line:" << endl << line << endl;
		return false;
	}
	in1 = curSym;
	const string in1_orig = in1;

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
	else if (!is_X(curSym, my_map,"input") && !is_X(curSym, my_map,"wire")) {
		cerr << "Error: " << curSym << " is not an input or a wire in following line:" << endl << line << endl;
		return false;
	}
	in2 = curSym;
	const string in2_orig = in2;

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
	else if (!is_X(curSym, my_map,"input") && !is_X(curSym, my_map,"wire")) {
		cerr << "Error: " << curSym << " is not an input or a wire in following line:" << endl << line << endl;
		return false;
	}
	in3 = curSym;
	const string in3_orig = in3;

	// end of line
	iss >> curSym;
	if (!iss.eof()) {
		cerr << "Error: expected end of line, not '" << curSym << "' in following line:" << endl << line << endl;
		return false;
	}
	bitManip(in1, my_map);
	bitManip(in2, my_map);
	bitManip(in3, my_map);

	vector<string> v{ in3_orig,in2_orig,in1_orig };
	graph.push_back(make_tuple("MUX2x1" + to_string(bitSize), v,out_orig, false ));
	oss << prefix + "MUX2x1#(" + to_string(bitSize) + ") Mux_" << ++numMux << "(" << in3 << "," << in2 << "," << in1 << "," << out << ");" << endl;

	return true;
}


int getBitSize(string output, const map<string, vector<string> > &my_map)
{
	string type;

	map<string, vector<string> >::const_iterator it = my_map.find(output);
	if (it == my_map.end())
		return 0;
	type = it->second[1];
	if (type[0] == 'I')
		type.erase(0, 3);
	else
		type.erase(0, 4);

	return stoi(type);
}

int getLgBit(string line, const map<string, vector<string> > &my_map)
{
	char curSym[20];
	string key;
	string type;
	int bitSize = 0;
	istringstream iss(line);
	iss >> curSym;  // throw away output only want largest input for comp
	while (iss >> curSym)
	{
		if (!isalpha(curSym[0]))
			continue;
		key = curSym;
		map<string, vector<string> >::const_iterator it = my_map.find(key);
		if (it == my_map.end())
			return 0;
		type = it->second[1];
		if (type[0] == 'I')
			type.erase(0, 3);
		else
			type.erase(0, 4);

		if (stoi(type) > bitSize)
			bitSize = stoi(type);
	}
	return bitSize;
}

bool signedVar(string key, const map<string, vector<string> > &my_map)
{
	map<string, vector<string> >::const_iterator it = my_map.find(key);
	if (it == my_map.end())
		return false;
	string type = it->second[1];
	if (type[0] == 'I')
		return true;
	else
		return false;
}

void bitManip(string &in1, const map<string, vector<string> > &my_map)
{
	if (!in1.empty())
	{
		if (signedVar(in1, my_map))
			in1 = "$signed(" + in1 + ")";
		else
			in1 = "$unsigned(" + in1 + ")";
	}
}
