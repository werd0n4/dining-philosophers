standard = 17
libraries = -pthread -lncurses
target = dinning-phils.o
flags = -g -std=c++$(standard)
files = main.cpp Fork.cpp Philosopher.cpp

$(target): $(files) 
	g++ main.cpp -o $(target) $(libraries) $(flags) 

all: $(target)

clean:
	rm -f *.o $(target)

dump: #pokaż kod w postaci assemblera
	g++ main.cpp -o $(target) $(libraries) $(flags) -S