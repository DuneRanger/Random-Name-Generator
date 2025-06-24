PROC_DIR=data_processing/

all: data/raw.csv data/processed.csv char_gen.hpp name_gen

.PHONY: all clean process

clean:
	rm .\gen_data_debug\*.csv
	rm .\data_processing\process_raw.exe
	rm .\data\processed.csv

data/processed.csv: data/raw.csv $(PROC_DIR)process_raw
	./$(PROC_DIR)process_raw

process: data/processed.csv data/raw.csv

$(PROC_DIR)process_raw: $(PROC_DIR)process_raw.cpp
	g++ $< -o $@

name_gen: name_generator.cpp
	g++ $< -o $@