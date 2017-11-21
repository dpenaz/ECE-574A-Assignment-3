#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

bool is_input(string sym, const map<string, vector<string> > &my_map);

bool signedVar(string key, const map<string, vector<string> > &my_map);
int getLgBit(string line, const map<string, vector<string> > &my_map);
int getBitSize(string output, const map<string, vector<string> > &my_map);
void bitManip(string &in1, const map<string, vector<string> > &my_map);

bool Add_or_INC(string line, const map<string, vector<string> > &my_map);
bool REG_(string line, const map<string, vector<string> > &my_map);
bool SUB_or_DEC(string line, const map<string, vector<string> > &my_map);
bool MOD_(string line, const map<string, vector<string> > &my_map);
bool DIV_(string line, const map<string, vector<string> > &my_map);
bool SHL_(string line, const map<string, vector<string> > &my_map);
bool SHR_(string line, const map<string, vector<string> > &my_map);
bool MUX2x1_(string line, const map<string, vector<string> > &my_map);
bool COMP_eq(string line, const map<string, vector<string> > &my_map);
bool COMP_lt(string line, const map<string, vector<string> > &my_map);
bool COMP_gt(string line, const map<string, vector<string> > &my_map);
bool MUL_(string line, const map<string, vector<string> > &my_map);