CC = g++ -Wall -mavx
CFLAGS = -Ilib/glfw/include -Ilib/vulkan/Include
LDFLAGS = -Llib/glfw/lib -Llib/vulkan/Lib -lglfw3 -lopengl32 -lgdi32 -luser32 -lvulkan-1

SRC = $(wildcard src/*.c)
OBJ  = $(SRC:.c=.o)
BIN = bin

.PHONY: all clean

all: game

run: all
	$(BIN)/Magi

game: $(OBJ)
	$(CC) -o $(BIN)/Magi $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	del /Q bin\*.exe src\*.o
