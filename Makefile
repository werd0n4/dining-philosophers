standard = 17
libraries = -pthread -lncurses
target = dinning-phils
flags = -g -std=c++$(standard)

$(target): main.cpp 
	g++ main.cpp -o $(target) $(libraries) $(flags) 

all: $(target)

clean:
	rm -f *.o $(target)

dump: #pokaż kod w postaci assemblera
	g++ main.cpp -o $(target) $(libraries) $(flags) -S