# Название финального исполняемого файла
TARGET = app

# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -fsanitize=address -fsanitize=leak -g

# Заголовки и исходники
INCLUDES = -I.
LIBS = -lcrypt

SRC = main.c db.c date_utils.c
OBJ = $(SRC:.c=.o)

# Основные правила
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJ) $(LIBS)

# Правило для запуска
run: all
	./$(TARGET)

# Очистка
clean:
	rm -f $(TARGET) *.o
