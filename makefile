OBJ_DIR = obj
INC_DIR = include
LIBS = -lrt
CFLAGS = -Wall -I $(INC_DIR) -g
OBJECTOS = main.o client.o driver.o restaurant.o memory.o process.o
main.o = main.h memory.h process.h 
client.o = client.h memory.h main.h
driver.o = driver.h memory.h main.h
restaurant.o = restaurant.h memory.h main.h
memory.o = memory.h memory-private.h
process.o = process.h driver.h client.h restaurant.h main.h memory.h 
CC = gcc

magnaeats: $(OBJECTOS)
	$(CC) $(addprefix $(OBJ_DIR)/,$(OBJECTOS)) -o bin/magnaeats $(LIBS)
%.o: src/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $<
clean:
	rm -f $(OBJ_DIR)/*.o bin/magnaeats
	rm -f bin/magnaeats