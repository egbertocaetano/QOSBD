all:    
	clang -c ./utils/util.c
	clang -c ./src/estimator.c
	clang -c ./src/main.c
	clang estimator.o util.o main.o -o qosdb
	rm *.o
