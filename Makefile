cc   = g++
out  = bin/yedit
libs = -lncurses
src  = src/main.cc
std  = c++17

build: $(src)
	@$(cc) $(src) -o $(out) $(libs) -std=$(std)

run: $(out)
	@./$(out)