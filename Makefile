CC = gcc
DEBUG = -g
CFLAGS = -Wall -Wextra 

PROG = shrtext

all: $(PROG)


$(PROG): $(PROG).o
	$(CC) $(DEBUG) $(CFLAGS) -o $@ $^

$(PROG).o: $(PROG).c
	$(CC) $(DEBUG) $(CFLAGS) -c $<


clean cls: 
	rm -f $(PROG) *.out *.err *.o *~ \
		*.txt *.bin *.dSYM \#*


ga: 
	git add $(PROG).c Makefile README.md

gc:
	@read -p "Enter commit messages: " msg; \
		git commit -m "$$msg"

gp: ga gc
	git push
