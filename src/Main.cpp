#include <iostream>
#include "Json.hpp"

int main() {
	using namespace std;

	try {
		Value v = parse(
			"{"
			"	\"hoge\" : \"str\","
			"	\"piyo\" : true,"
			"	\"fuga\" : [ 1.0e+5 , 0.04E2 ] ,"
			"	\"hogera\" : {}"
			"}"
		);
		cout << v.stringify() << endl;
	}
	catch(const exception& e) {
		cout<< typeid(e).name() << endl
			<< e.what() << endl;
	}
}
