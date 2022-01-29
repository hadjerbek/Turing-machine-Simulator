# Définitions de macros
CC = gcc
CFLAGS = -c -Wall
LFLAGS = -lreadline
CSRC = ruban.c machineturing.c main.c
EXEC = simulation_mt

OBJ = $(CSRC:.c=.o)

# .PHONY run :pour dire que même s'il y a un fichier run,
# on en tient pas compte
.PHONY: run

run: $(EXEC)
	./$(EXEC)

test: $(EXEC)
	./$(EXEC) -C codes_machines_turing/odd_number_of_b codes_machines_turing/odd_number_of_b_bin ; \
  ./$(EXEC) codes_machines_turing/ajout_1_a_nb_binaire 01:01 "_" ; \
  ./$(EXEC) codes_machines_turing/binary_palindrome 01:01 "_" ; \
  ./$(EXEC) codes_machines_turing/even_number_of_zeros 01:01 "_" ; \


$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) $<

%.o: %.c
	$(CC) $(CFLAGS) $<


.PHONY: clean
clean:
	rm -f *.o
	rm -f $(EXEC)
	ls -l