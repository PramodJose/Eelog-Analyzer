PROG_NAME = analyzer.c
EXEC_NAME = analyzer.out

all: $(PROG_NAME)
	gcc $(PROG_NAME) -o $(EXEC_NAME)
run:
	./$(EXEC_NAME)
clean:
	rm $(EXEC_NAME)
