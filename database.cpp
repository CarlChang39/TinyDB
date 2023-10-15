#include "database.h"

void Database::start() {
	while (true) {
		cout << "db > ";

		string input;
		getline(cin, input);

		if (input == "exit") {
			cout << "bye!" << endl;
			exit(0);
		}
	}
}