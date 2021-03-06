PROG_NAME = analyzer.c
EXEC_NAME = analyzer.out

all: $(PROG_NAME)
	gcc $(PROG_NAME) -o $(EXEC_NAME)
run:
	gcc $(PROG_NAME) -o $(EXEC_NAME)
	./$(EXEC_NAME)
run10:
	gcc $(PROG_NAME) -o $(EXEC_NAME)
	./$(EXEC_NAME) -t 10
run100:
	gcc $(PROG_NAME) -o $(EXEC_NAME)
	./$(EXEC_NAME) -t 100
clean:
	rm $(EXEC_NAME)
