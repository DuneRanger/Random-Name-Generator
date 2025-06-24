#pragma once
#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <fstream>
#include <random>

// + 1 for an end-of-word character (chosen to be the ` backtick character)
constexpr uint8_t ALPHABET_SIZE = 26 + 1;

std::string lowercase(const std::string& word) {
	std::string lower = "";
	for (int i = 0; i < word.size(); i++) {
		if (word[i] >= 'A' && word[i] <= 'Z') lower += ('a' + word[i] - 'A');
		else lower += word[i];
	}
	return lower;
}

// Generates letters based on input data and a prefix given during generation
class CharGen {
	private:
		// pairs of letters and their *base* count from the data
		std::array<uint32_t, ALPHABET_SIZE> letters_raw;

		// char_cnt for prefixes of length 1-2 in the base name
		// used to increase the probability of letters following common prefixes
		std::unordered_map<std::string, std::array<uint32_t, ALPHABET_SIZE>> prefixes;
		uint8_t max_prefix_len = 2;
		uint8_t len_prefix_mult = 4;
		uint64_t base_prefix_mult = UINT16_MAX;

		char calc_char_ind(char c) {
			if (c == '$') return 0;
			else return c - '`';
		}

		char char_from_ind(int ind) {
			if (ind == 0) return '$';
			else return char('`' + ind);
		}

		std::string prepare_word(const std::string& _word) {
			std::string prefix = "";
			for (int i = 0 ; i < max_prefix_len; i++) prefix += "^";
			return (prefix + lowercase(_word) + "$");
		}

	public:
		CharGen() {
			for (int i = 0; i < letters_raw.size(); i++) letters_raw[i] = 0;
		}

		void set_max_prefix_len(uint8_t _max_prefix_len) {
			max_prefix_len = _max_prefix_len;
		}

		void set_len_prefix_mult(uint8_t _len_prefix_mult) {
			len_prefix_mult = _len_prefix_mult;
		}

		void set_base_prefix_mult(uint8_t _base_prefix_mult) {
			base_prefix_mult = _base_prefix_mult;
		}

		// overwrites generation data with a list of words given as input
		void import_words(const std::vector<std::string>& words) {
			for (std::string word : words) {
				word = prepare_word(word);
				for (int i = max_prefix_len; i < word.size(); i++) {
					char char_ind = calc_char_ind(word[i]); 
					letters_raw.at(char_ind) += 1;

					for (int prefix_len = 1; prefix_len <= max_prefix_len; prefix_len++) {
						std::string prefix = word.substr(i-prefix_len, prefix_len);
						if (!prefixes.count(prefix)) {
							prefixes[prefix] = std::array<uint32_t, ALPHABET_SIZE>();
							for (int i = 0; i < prefixes[prefix].size(); i++) prefixes[prefix][i] = 0;
						}
						prefixes[prefix][char_ind] += 1;
					}
				}
			}
		}
		// overwrites generation data with a list of words given as input
		// char count and prefixes will only be calculated for the given index of the words
		void import_words_positional(const std::vector<std::string>& words, uint8_t ind) {
			for (std::string word : words) {
				if (ind >= word.size()) continue;
				word = prepare_word(word);
				ind += max_prefix_len;
				char char_ind = calc_char_ind(word[ind]); 
				letters_raw.at(char_ind) += 1;

				for (int prefix_len = 1; prefix_len <= max_prefix_len; prefix_len++) {
					std::string prefix = word.substr(ind-prefix_len, prefix_len);
					if (!prefixes.count(prefix)) {
						prefixes[prefix] = std::array<uint32_t, ALPHABET_SIZE>();
						for (int i = 0; i < prefixes[prefix].size(); i++) prefixes[prefix][i] = 0;
					}
					prefixes[prefix][char_ind] += 1;
				}
			}
		}

		// Adds the characters of the word given as input into the data
		void add_word(const std::string& _word) {
			std::string word = prepare_word(_word);
			for (int i = max_prefix_len; i < word.size(); i++) {
				char char_ind = calc_char_ind(word[i]); 
				letters_raw.at(char_ind) += 1;

				for (int prefix_len = 1; prefix_len <= max_prefix_len; prefix_len++) {
					std::string prefix = word.substr(i-prefix_len, prefix_len);
					if (!prefixes.count(prefix)) {
						prefixes[prefix] = std::array<uint32_t, ALPHABET_SIZE>();
						for (int i = 0; i < prefixes[prefix].size(); i++) prefixes[prefix][i] = 0;
					}
					prefixes[prefix][char_ind] += 1;
				}
			}
		}
		// Adds the characters of the word given as input into the data
		// char count and prefixes will only be calculated for the given index of the word
		void add_word_positional(const std::string& _word, uint8_t ind) {
			if (ind >= _word.size()+1) return;
			std::string word = prepare_word(_word);
			ind += max_prefix_len;
			char char_ind = calc_char_ind(word[ind]);
			letters_raw.at(char_ind) += 1;

			for (int prefix_len = 1; prefix_len <= max_prefix_len; prefix_len++) {
				std::string prefix = word.substr(ind-prefix_len, prefix_len);
				if (!prefixes.count(prefix)) {
					prefixes[prefix] = std::array<uint32_t, ALPHABET_SIZE>();
					for (int i = 0; i < prefixes[prefix].size(); i++) prefixes[prefix][i] = 0;
				}
				prefixes[prefix][char_ind] += 1;
			}
		}

		// writes the raw character count and prefix data to a csv
		void export_to_csv(const std::string& _path) {
			std::string path = _path;
			if (path.size() == 0) path = "CharGen_data.csv";
			else if (path.size() < 3 || path.substr(path.size()-4) != ".csv") {
				if (path[path.size() - 1] == '\\' || path[path.size() - 1] == '/') path += "CharGen_data";
				path += ".csv";
			}
			std::ofstream output(path);
			output << "prefix,char,count\n";
			for (int i = 0; i < letters_raw.size(); i++) {
				if (letters_raw[i] == 0) continue;
				output << ',' << char_from_ind(i) << ',' << letters_raw[i] << '\n';
			}
			for (auto& it : prefixes) {
				for (int i = 0; i < it.second.size(); i++) {
					if (it.second[i] == 0) continue;
					output << it.first << ',' << char_from_ind(i) << ',' << it.second[i] << '\n';
				}
			}
			output.close();
		}

		// generates a character based on internal data and a prefix (if given)
		char gen_char(std::string _prefix = "") {
			std::string prefix = "";
			for (int i = 0; i < max_prefix_len; i++) prefix += '^';
			prefix += _prefix;
			std::array<uint64_t, ALPHABET_SIZE> letters;
			uint64_t total_count = 0;
			for (int i = 0; i < letters_raw.size(); i++) {
				letters[i] = letters_raw[i];
				total_count += letters_raw[i];
			}
			if (prefix.size() > max_prefix_len) prefix = prefix.substr(prefix.size() - max_prefix_len);
			uint64_t prefix_mult = base_prefix_mult << (len_prefix_mult*max_prefix_len);
			for (int i = max_prefix_len; i > 0; i--) {
				prefix_mult >>= len_prefix_mult;
				if (prefix.size() < i) continue;
				if (!prefixes.count(prefix)) continue;
				for (int j = 0; j < prefixes[prefix].size(); j++) {
					uint64_t extra_letters = prefixes[prefix][j] * prefix_mult;
					total_count += extra_letters;
					letters[j] += extra_letters;
				}
			}
			// std::ofstream test("./testing/letters_" + prefix + ".csv");
			// test << prefix << '\n';
			// for (int i = 0; i < letters.size(); i++) {
			// 	test << char_from_ind(i) << ',' << letters[i] << '\n';
			// }
			// test.close();
			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<int64_t> uniform(1, total_count);
			int64_t num = uniform(gen);
			// std::cout << num << " " << total_count << " " << UINT32_MAX  << std::endl;
			for (int i = 0; i < letters.size(); i++) {
				// std::cout << i << " : " << num << " " << letters[i] << " | ";
				num -= letters[i];
				if (num <= 0) return char_from_ind(i);
			}
			return '!';
		}

		std::string gen_word(std::string prefix = "") {
			std::string word = prefix;
			char next = gen_char(word);
			while (next != '$') {
				word += next;
				next = gen_char(word);
			}
			if (word.size() > 2) return word;
			else return gen_word();
		}
};