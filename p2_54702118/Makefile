EXEC=p2_exec
CC=gcc
CC=g++
CFLAGS=-I.


#----------------------------------------------
# complete this part
%.o: %.cpp	
	$(CC) -c $< $(CFLAGS)
#----------------------------------------------

#----------------------------------------------
# complete this part
${EXEC}: main.o  types_p2.o p2_threads.o utils.o
	g++ -o ${EXEC} main.o types_p2.o  p2_threads.o utils.o -I. -lpthread
#----------------------------------------------


clean:
	rm -rf ./${EXEC}
	rm -rf ./*.o
