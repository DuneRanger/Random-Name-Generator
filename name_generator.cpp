#include <array>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include "char_gen.hpp"


class NameGen {
	private:
		// max length of data saved (and name length)
		static const uint8_t max_len = 15;
		// generator for masculine names, where each char generated accounts for their position in the name
		std::array<CharGen, max_len> masculine_gen;
		// generator for feminine names, where each char generated accounts for their position in the name
		std::array<CharGen, max_len> feminine_gen;
		// A generator for all names, where letters are chosen irrespective of their position in the name 
		CharGen global_gen;

		uint8_t gen_max_prefix_len = 2;
		uint8_t gen_len_prefix_mult = 4;
		uint64_t gen_base_prefix_mult = UINT16_MAX;

	public:
		NameGen() {}

		void setup(std::string path, uint8_t max_prefix_len = 2, uint8_t len_prefix_mult = 4, uint64_t base_prefix_mult = UINT16_MAX) {
			for (int i = 0; i < max_len; i++) {
				masculine_gen[i].set_max_prefix_len(max_prefix_len);
				masculine_gen[i].set_len_prefix_mult(len_prefix_mult);
				masculine_gen[i].set_base_prefix_mult(base_prefix_mult);
				feminine_gen[i].set_max_prefix_len(max_prefix_len);
				feminine_gen[i].set_len_prefix_mult(len_prefix_mult);
				feminine_gen[i].set_base_prefix_mult(base_prefix_mult);
			}
			global_gen.set_max_prefix_len(max_prefix_len);
			global_gen.set_len_prefix_mult(len_prefix_mult);
			global_gen.set_base_prefix_mult(base_prefix_mult);

			std::ifstream input(path);
			std::string line;
			input >> line;
			while (input >> line) {
				int comma = line.find(',');
				std::string masc = line.substr(0, comma);
				std::string fem = line.substr(comma+1);
				for (int i = 0; i < max_len; i++) {
					masculine_gen[i].add_word_positional(masc, i);
				}
				for (int i = 0; i < max_len; i++) {
					feminine_gen[i].add_word_positional(fem, i);
				}
				if (masc.size()) global_gen.add_word(masc);
				if (fem.size()) global_gen.add_word(fem);
			}
			// std::string base_path = "./testing/";
			// for (int i = 0; i < max_len; i++) {
			// 	masculine_gen[i].export_to_csv(base_path + std::to_string(i) + ".csv");
			// }
		}
		// generates a masculine name, length can be limited
		std::string gen_masc(uint8_t max_length = max_len) {
			std::string name = "";
			char next = masculine_gen[0].gen_char(name);
			while (next != '$') {
				name += next;
				if (name.size() >= max_len) break;
				next = masculine_gen[name.size()].gen_char(name);
			}
			if (name.size() > 2) return name;
			else return gen_masc(max_length);
		}
		// generates a feminine name, length can be limited
		std::string gen_fem(uint8_t max_length = max_len) {
			std::string name = "";
			char next = feminine_gen[0].gen_char(name);
			while (next != '$') {
				name += next;
				if (name.size() >= max_len) break;
				next = feminine_gen[name.size()].gen_char(name);
			}
			if (name.size() > 2) return name;
			else return gen_fem(max_length);
		}
		// generates either a masculine or feminine name, based on 50/50 odds
		// length can be limited
		std::string gen(uint8_t max_length = max_len) {
			if (std::rand()%2) return gen_masc();
			return gen_fem();
		}

		// generates a name, where each character is generated irrespective of their position in the name
		// the data used contains both masculine and feminine names
		// length can be limited
		std::string gen_non_positional(uint8_t max_length = max_len) {
			return global_gen.gen_word();
		}
};

int load_num(char *str) {
	int num = 0;
	for (int j = 0; str[j]; j++) {
		if (str[j] < '0' || str[j] > '9') continue;
		num *= 10;
		num += str[j] - '0';
	}
	return num;
}

void print_help() {
	std::cout << "===================================================================\n";
	std::cout << "This program only supports single letter options.\n";
	std::cout << "-h \tDisplays this help message.\n";
	std::cout << "-m \tGenerates masculine names based on the generation count.\n";
	std::cout << "-f \tGenerates feminine names based on the generation count.\n";
	std::cout << "\t \tNote: if both -m and -f are active at once, then the total number of generated names will be twice the generation count.\n";
	std::cout << "If neither are chosen, then every name has its gender chosen 50/50.\n";
	std::cout << "-c [number] \tChanges the name generation count.\n";
	std::cout << "-l [number] \tChanges the maximum length of prefixes to be considered during generation. The default value is 2.\n";
	std::cout << "\t \tA higher number makes the name generated to be more similar to existing names, however it drastically increases the initial preparation time.";
	std::cout << "-e [number] \tChanges the prefix-length exponent. The default value is 4 (i.e. every prefix has a weight of 2^[length].\n";
	std::cout << "-b [number] \tChanges the base prefix multiplier. The default values is 65535. Decreasing this value can make names more random\n";
	std::cout << "===================================================================\n";
}

int main(int argc, char *argv[]) {
	bool masc = false;
	bool fem = false;
	int gen_count = 1;
	uint8_t max_prefix_len = 2;
	uint8_t len_prefix_mult = 4;
	uint64_t base_prefix_mult = UINT16_MAX;

	for (int i = 0; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1]) {
			switch (argv[i][1]) {
				case 'm':
					masc = true;
					break;
				case 'f':
					fem = true;
					break;
				case 'c':
					gen_count = load_num(argv[++i]);
					break;
				case 'l':
					max_prefix_len = load_num(argv[++i]);
					break;
				case 'e':
					len_prefix_mult = load_num(argv[++i]);
					break;
				case 'b':
					base_prefix_mult = load_num(argv[++i]);
					break;
				case 'h':
					print_help();
					return 0;
					break;
				default:
					std::cout << "Error: Option not recognised\n";
					print_help();
					return 1;
			}
		}
	}

	NameGen gen;
	std::string path = "./data/processed.csv";
	gen.setup(path, max_prefix_len, len_prefix_mult, base_prefix_mult);
	if (masc) {
		for (int i = 0; i < gen_count; i++) std::cout << gen.gen_masc() << " ";
		std::cout << std::endl;
	}
	if (fem) {
		for (int i = 0; i < gen_count; i++) std::cout << gen.gen_fem() << " ";
		std::cout << std::endl;
	}
	if (!masc && !fem) {
		for (int i = 0; i < gen_count; i++) std::cout << gen.gen() << " ";
		std::cout << std::endl;
	}
}