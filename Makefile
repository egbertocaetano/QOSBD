all: 	
		gcc -o ./utils/util.c
		gcc -o ./src/estimator.c
		gcc -o ./src/main.c
		gcc estimator.o util.o main.o -o main
