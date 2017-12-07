#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

Node* assign_op_result(string op, string line, const map<string, vector<string> > &my_map);

Node* REG_(string line, const map<string, vector<string> > &my_map);
Node* MUX2x1_(string line, const map<string, vector<string> > &my_map);