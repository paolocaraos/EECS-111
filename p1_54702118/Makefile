EXEC=p1_exec
CC=gcc

CC=g++
CFLAGS=-I.


%.o: %.cpp
	$(CC) -c $< $(CFLAGS)

${EXEC}: main.o p1_process.o p1_threads.o
	g++ -o ${EXEC} main.o p1_process.o p1_threads.o -I. -lpthread 



clean:
	rm -rf ./${EXEC}
	rm -rf ./*.o
