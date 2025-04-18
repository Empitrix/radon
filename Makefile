all:
	@ cc -o output -lm -lraylib ./main.c

run:
	@ ./output

total:
	@ $(MAKE) all --no-print-directory
	@ $(MAKE) run --no-print-directory

