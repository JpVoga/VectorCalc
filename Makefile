COMPILER = g++
BASE_FLAGS = -std=c++2b -Wall -Wextra
DEBUG = 1

ifneq ($(DEBUG), 0) # If debug is not false (it's true)
FLAGS = $(BASE_FLAGS) -O0 -D_DEBUG=1 -UNDEBUG -g

else
FLAGS = $(BASE_FLAGS) -O3 -D_DEBUG=0 -DNDEBUG
endif

app.exe: $(wildcard src/**/*) $(wildcard src/*) Makefile
	$(COMPILER) src/app.cpp -o app.exe $(FLAGS)