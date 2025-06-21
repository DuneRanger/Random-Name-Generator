#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <unordered_map>

typedef std::pair<char, uint32_t> char_cnt;

// Generates letters based on input data and a prefix given during generation
class CharGen {
	private:
		// pairs of letters and their *base* count from the data
		std::vector<char_cnt> letters_raw;

		// char_cnt for prefixes of length 1-2 in the base name
		// used to increase the probability of letters following common prefixes
		std::unordered_map<std::string, std::vector<char_cnt>> prefixes;
		const uint8_t max_prefix_len = 2;

		// pairs of letters and their *modified* count from the data
		std::vector<char_cnt> letters;

	public:
		// overwrites generation data with a list of words given as input
		// if an index is given, then char count and prefixes will only be calculated for the given index
		void import_words(std::vector<std::string> words, char index = -1);

		// Adds the characters of the word given as input into the data
		// if an index is given, then char count and prefixes will only be calculated for the given index
		void add_word(std::string word, char index);
		// writes all internal character count data to a csv
		void export_to_csv(std::string path);

		// generates a character based on internal data and a prefix (if given)
		char gen(std::string prefix = "");
};

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

	public:
		// generates a masculine name, length can be limited
		std::string gen_masc(uint8_t max_length = max_len);
		// generates a feminine name, length can be limited
		std::string gen_fem(uint8_t max_length = max_len);
		// generates either a masculine or feminine name, based on 50/50 odds
		// length can be limited
		std::string gen(uint8_t max_length = max_len);

		// generates a name, where each character is generated irrespective of their position in the name
		// the data used contains both masculine and feminine names
		// length can be limited
		std::string gen_non_positional(uint8_t max_length = max_len);
};