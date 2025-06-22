INPUT = main.c
OUTPUT = radon
COMPILER = cc
FLAGS = -lraylib

compile:
	@ $(COMPILER) -o $(OUTPUT) $(FLAGS) $(INPUT)

run:
	@ ./$(OUTPUT)

quick:
	@ $(MAKE) compile --no-print-directory
	@ $(MAKE) run --no-print-directory

