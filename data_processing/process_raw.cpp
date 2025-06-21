#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::ifstream raw("data/raw.csv");
std::ofstream processed("data/processed.csv");

bool is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
} 

bool is_valid_name(std::string& name) {
	for (int i = 0; i < name.size(); i++) {
		if (!is_alpha(name[i])) return false;
	}
	return true;
}

void divide_by_gender(std::vector<std::string>& masculine, std::vector<std::string>& feminine) {
	std::string line;
	raw >> line;
	while (raw >> line) {
		if (line.size() < 3) continue;
		char gender = line[0];
		std::string name = line.substr(2);
		if (!is_valid_name(name)) continue;
		switch (gender) {
		case 'f':
			feminine.push_back(name);
			break;
		case 'm':
			masculine.push_back(name);
			break;
		default:
			break;
		}
	}
}

int main(int argc, char *argv[]) {
	std::vector<std::string> masculine_names;
	std::vector<std::string> feminine_names;
	
	divide_by_gender(masculine_names, feminine_names);

	processed << "masculine,feminine\n";
	int ind = 0;
	for (; ind < masculine_names.size(); ind++) {
		processed << masculine_names[ind] << ",";
		if (ind < feminine_names.size()) processed << feminine_names[ind];
		processed << '\n';
	}
	for (; ind < feminine_names.size(); ind++) {
		processed << "," << feminine_names[ind] << '\n';
	}
	return 0;
}