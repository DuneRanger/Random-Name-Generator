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

int main(int argc, char *argv[]) {
	NameGen gen;
	std::string path = "./data/processed.csv";
	gen.setup(path);
	for (int j = 0; j < 5; j++) {
		for (int i = 0; i < 10; i++) {
			std::cout << gen.gen() << "\t";
		}
		std::cout << '\n';
	}
	std::cout << std::endl;
	
}