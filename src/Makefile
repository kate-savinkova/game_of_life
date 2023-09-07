GCC = gcc -Wall -Wextra -Werror 
PATH = ../bin/game

all: game
clean: 
	rm -rf *.out *.o $(PATH)
rebuild: clean all

game:  game_of_life.c
	$(GCC) $^ -o $(PATH) 

run1: $(PATH)
	./$(PATH) < 1.txt
run2: $(PATH)
	./$(PATH) < 2.txt
run3: $(PATH)
	./$(PATH) < 3.txt
run4: $(PATH)
	./$(PATH) < 4.txt
run5: $(PATH)
	./$(PATH) < 5.txt

