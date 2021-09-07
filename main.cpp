
#include <vector>
#include <ncurses.h>
#include <thread>
#include <condition_variable>
#include <ctime>
#include <cstdlib>
#include <atomic>

#include "Philosopher.cpp"

bool running, isStarted;
std::mutex refresh_mtx;

WINDOW** init_screen()
{
    initscr();   
    curs_set(0);
    //colors
    start_color();
    init_color(COLOR_YELLOW, 800, 800, 0);
    init_pair(Colors::green, COLOR_GREEN, COLOR_BLACK);//thinking
    init_pair(Colors::red, COLOR_RED, COLOR_BLACK);//eating
    init_pair(Colors::yellow, COLOR_YELLOW, COLOR_BLACK);//waiting
    init_pair(Colors::cyan, COLOR_CYAN, COLOR_BLACK);//input frames
    init_pair(Colors::white, COLOR_WHITE, COLOR_BLACK);//

    const int y_max_size = getmaxy(stdscr);
    const int x_max_size = getmaxx(stdscr);
    
    WINDOW* inputWin = newwin(3, x_max_size/2, 0, 0);
    wattron(inputWin, COLOR_PAIR(Colors::cyan));
    box(inputWin, 0, 0);
    wattroff(inputWin, COLOR_PAIR(Colors::cyan));
    wmove(inputWin, 1, 1);
    wprintw(inputWin, "How many philosophers will be eating(should be >= 5): ");

    WINDOW* exitWin = newwin(3, x_max_size/2, 0, x_max_size/2);
    wattron(exitWin, COLOR_PAIR(Colors::cyan));
    box(exitWin, 0, 0);
    wattroff(exitWin, COLOR_PAIR(Colors::cyan));
    wmove(exitWin, 1, 1);
    wprintw(exitWin, "To exit press q. (Program will stop all running threads first): ");

    WINDOW** windows = new WINDOW* [2];
    windows[0] = inputWin;
    windows[1] = exitWin;

    wrefresh(inputWin);
    wrefresh(exitWin);

    return windows;
}

void user_input(WINDOW** windows, int& philsNmb)
{
    char* input;
    while(running){
        if(!isStarted){
            mvwscanw(windows[0], 1, 55,"%d", &philsNmb);
            isStarted = true;
        }
        else{
            char input = wgetch(windows[1]);
            if(input == 'q'){
                running = false;
            }
        }
    }
}

int main()
{
    srand(time(NULL));
    int philsNmb;
    std::vector<Fork> forks;
    std::vector<Philosopher> philosophers;
    std::vector<std::thread> philsThreads;

    running = true;
    isStarted = false;
    WINDOW** windows = init_screen();
    
    std::thread userInput([&windows, &philsNmb]{user_input(windows, philsNmb);});

    while(true){
        if(isStarted){
            forks = std::vector<Fork>(philsNmb);
            for(int i = 0; i < philsNmb; ++i){
                //forks init
                forks[i].free = true;

                //phils init
                philosophers.push_back(Philosopher(forks, i, philsNmb));
            }            
            break;
        }
    }

    for(auto& phil : philosophers )
    {
        philsThreads.push_back(std::thread([&phil]{phil.feast();}));
    }
    for(auto& thread : philsThreads)
    {
        thread.join();
    }
    userInput.join();

    endwin();

    return 0;
}