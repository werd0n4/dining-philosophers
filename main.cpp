#include <iostream>
#include <vector>
#include <ncurses.h>
#include <thread>
#include <condition_variable>
#include <ctime>
#include <cstdlib>

#include "Philosopher.cpp"
// #include "Fork.cpp"

bool running, isStarted;
int x_max_size=0;
std::mutex refresh_mtx;

WINDOW** init_screen()
{
    initscr();   
    curs_set(0);
    //colors
    start_color();
    init_color(COLOR_YELLOW, 800, 800, 0);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);//thinking
    init_pair(2, COLOR_RED, COLOR_BLACK);//eating
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);//waiting
    init_pair(4, COLOR_CYAN, COLOR_BLACK);//main frames
    init_pair(5, COLOR_WHITE, COLOR_BLACK);//

    int y_max_size;
    getmaxyx(stdscr, y_max_size, x_max_size);
    WINDOW* inputWin = newwin(3, x_max_size/2, 0, 0);
    WINDOW* exitWin = newwin(3, x_max_size/2, 0, x_max_size/2);
    wattron(inputWin, COLOR_PAIR(4));
    box(inputWin, 0, 0);
    wattroff(inputWin, COLOR_PAIR(4));
    wattron(exitWin, COLOR_PAIR(4));
    box(exitWin, 0, 0);
    wattroff(exitWin, COLOR_PAIR(4));
    wmove(inputWin, 1, 1);
    wprintw(inputWin, "How many philosophers will be eating(should be >= 5): ");
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

                //phils
                philosophers.push_back(Philosopher(forks, i, x_max_size, philsNmb));
            }            
            break;
        }
    }

    for(auto it = philosophers.begin(); it != philosophers.end(); ++it)
    {
        philsThreads.push_back(std::thread([it]{it->feast();}));
    }
    for(auto it = philsThreads.begin(); it != philsThreads.end(); ++it)
    {
        (*it).join();
    }
    userInput.join();

    endwin();

    return 0;
}