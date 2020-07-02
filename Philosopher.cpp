#include "Fork.cpp"
 

class Philosopher
{
    public:
    int chairId, window_width;
    float currentTime, baseTime = 4.0;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point now;
    std::chrono::duration<double> diff;
    // enum State{ think, hungry, eat};
    // State state;
    std::string state;
    std::vector<Fork>& forks;
    WINDOW* statusWin;    
    WINDOW* progresWin;    
    WINDOW* forksWin;    

    public:
    Philosopher(std::vector<Fork>& _forks, int _chairId, int _window_width) : forks(_forks), chairId(_chairId), window_width(_window_width) {
        statusWin = newwin(3, window_width/4, 3*(chairId + 1), 0);
        progresWin = newwin(3, window_width/4, 3*(chairId + 1), window_width/4);
        forksWin = newwin(3, window_width/4, 3*(chairId + 1), window_width/2);
        box(statusWin, 0, 0);
        box(progresWin, 0, 0);
        box(forksWin, 0, 0);
        // refresh();
        wmove(statusWin, 1, 1);
        wmove(progresWin, 1, 1);
        wmove(forksWin, 1, 1);
        // wrefresh(statusWin);
        // wrefresh(progresWin);
        // wrefresh(forksWin);
        // wmove(statusWin, 1, 1);
        // wmove(progresWin, 1, 1);
        // wmove(forksWin, 1, 1);
    }

    void thinking(){
        wprintw(statusWin, "Philosopher nr %d ", chairId);
        // wprintw(statusWin, "THINKING");
        // wrefresh(statusWin);
        // currentTime = baseTime + (float)rand()/RAND_MAX;

        // start = std::chrono::high_resolution_clock::now();
        // while(diff.count() < currentTime){
        //     // werase(progresWin);
        //     now = std::chrono::high_resolution_clock::now();
        //     diff = now - start;
        //     wprintw(progresWin, std::to_string(diff.count()).c_str());
        //     // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // }
    }

    void eating(){

    }

    void get_forks(){

    }

    void put_forks(){

    }

    void feast(){
        thinking();
    }

    void draw_window(){

    }
};