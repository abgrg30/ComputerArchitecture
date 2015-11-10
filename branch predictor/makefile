CC=g++
OPTS=-g

all: main.o traceread.o predictor.o
	$(CC) $(OPTS) -lm -o predictor main.o traceread.o predictor.o

main.o: main.C traceread.h predictor.h
	$(CC) $(OPTS) -c main.C

traceread.o: traceread.h traceread.C
	$(CC) $(OPTS) -c traceread.C

predictor.o: predictor.h predictor.C
	$(CC) $(OPTS) -c predictor.C

clean:
	rm -f *.o predictor;
