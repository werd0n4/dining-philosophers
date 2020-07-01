#include "Fork.cpp"

class Philosopher
{
    public:
    int chairId, window_width;
    enum State{ think, hungry, eat};
    State state;
    // std::vector<Fork> forks;
    WINDOW* statusWin;    
    WINDOW* progresWin;    
    WINDOW* forksWin;    

    public:
    Philosopher(std::vector<Fork>& forks, int _chairId, int _window_width) : chairId(_chairId), window_width(_window_width) {
        statusWin = newwin(3, window_width/4, 3*(chairId + 1), 0);
        progresWin = newwin(3, window_width/4, 3*(chairId + 1), window_width/4);
        forksWin = newwin(3, window_width/4, 3*(chairId + 1), window_width/2);
        box(statusWin, 0, 0);
        box(progresWin, 0, 0);
        box(forksWin, 0, 0);
        // refresh();
        // wrefresh(statusWin);
        // wrefresh(progresWin);
        // wrefresh(forksWin);
    }

    void thinking(){

    }

    void eating(){

    }

    void get_forks(){

    }

    void put_forks(){

    }

    void feast(){
        
    }

    void draw_window(){

    }
};