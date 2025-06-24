# Random Name Generator

A small project I made for fun.
It includes a basic statistics-based character generator and a name generator utilising it.

## Data processing

This section is heavily focused for just the name generator.
[Here](https://w.wiki/EXvc) is the Wikidata query used to gather the raw data,
which is then just combined into a shorter `.csv` file by `process_raw.cpp`.

## Character generator

Found in `char_gen.hpp`. It takes a list of words or a single word, and optionally an index/position to focus on,
and for each letter and every prefix leading up to the letter (capped by `max_prefix_len` - default=2)
it counts how many occurrences there are.
Then, based on the occurrences and the length of an optional matching prefix, it randomly chooses a character to generate.
The base prefix multiplier is `UINT16_MAX` and additional prefix length increase the multiplier by 16 times.
Note that all of these prefix parameters are configurable during run-time.

It can also generate an end-of-word character, chosen to be `$`.
Additionally, it internally uses `^` as a prefix symbol to symbolise the start of a word,
so it would be wise not to include these two characters in the input data.

## Name Generator

It consists of an individual character generator for 15 possible positions in the word, thus limiting the name length to 15.
This ensures that the names generated typically contain "normal" starts of names or ends of names.

The processed input data consists of around 1000 feminine names and 1900 masculine names, all of which consist of a single word
(i.e. the names are just single names) and characters from the english alphabet.
All of the names are added into different character generators based on gender, because names of a single gender tend to share
more patterns than names of a different gender.

There is also a character generator which is fed names from both genders, however it doesn't generate characters by position,
meaning that it generates random-looking names, that may only be very vaguely similar to existing names.

### Usage

You can generate `name_gen.exe` by using `make` in the command line, or alternatively by having your own `processed.csv` data file
and compiling `name_generator.cpp` manually.




