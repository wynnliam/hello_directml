// Liam Wynn, 5/29/2024, Hello DirectML

/*
	A simple application for learning Direct ML's features.
	This will implement a really simple idenity function:

	f(x) = x.

	But the hope is that this proves to be useful in constructing
	richer DirectML programs. This program will be based on the
	HelloDirectML program from Microsoft:

	https://github.com/microsoft/DirectML/blob/master/Samples/HelloDirectML/

	However I will structure mine more closely after my own Hello DirectX 12
*/

#include "dml_handler.h"
#include <iostream>

using namespace std;

int main() {
	dml_handler* dml;

	dml = new dml_handler;
	if (!initialize_dml_handler(dml)) {
		cerr << "Failed to initialize DirectML!" << endl;
		return -1;
	}

	cout << "Welcome to Direct ML!" << endl;

	return 0;
}