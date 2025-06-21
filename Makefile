DATA = \
raw \
processed \

DATA_CSV = $(addsuffix .csv, $(DATA))
DATA_FULL = $(addprefix data/, $(DATA_CSV))

PROC_DIR=data_processing/

all: process

.PHONY: all clean process

clean:

data/processed.csv: process

process: data/raw.csv $(PROC_DIR)process_raw
	./$(PROC_DIR)process_raw

$(PROC_DIR)process_raw: $(PROC_DIR)process_raw.cpp
	g++ $< -o $@