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
    int y_max_size;
    getmaxyx(stdscr, y_max_size, x_max_size);
    WINDOW* inputWin = newwin(3, x_max_size/2, 0, 0);
    WINDOW* exitWin = newwin(3, x_max_size/2, 0, x_max_size/2);
    box(inputWin, 0, 0);
    box(exitWin, 0, 0);
    wmove(inputWin, 1, 1);
    wprintw(inputWin, "How many philosophers will be eating(should be >= 5): ");
    wmove(exitWin, 1, 1);
    wprintw(exitWin, "To exit press q. (Program will stop all running threads first): ");

    WINDOW** windows = new WINDOW* [2];
    windows[0] = inputWin;
    windows[1] = exitWin;

    refresh();
    wrefresh(windows[0]);
    wrefresh(windows[1]);

    return windows;
}

void refresh_screen(std::vector<Philosopher>& phils, std::vector<Fork>& forks)
{
    while(running){
        refresh();
        for(auto it = phils.begin(); it != phils.end(); ++it){
            wrefresh((*it).forksWin);
            wrefresh((*it).statusWin);
            wrefresh((*it).progresWin);
        }
        for(auto it = forks.begin(); it != forks.end(); ++it){
            wrefresh((*it).window);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void user_input(WINDOW** windows, int& philsNmb)
{
    char* input;
    while(running){
        if(!isStarted){
            mvwscanw(windows[0], 1, 55,"%d", &philsNmb);
            isStarted = true;
            // wmove(windows[1], 1, 65);
        }
        else{
            char input = wgetch(windows[1]);
            if(input == 'q'){
                running = false;
            }
        }
    }
}

void end_screen()
{
    endwin();
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
                forks[i].window = newwin(3, x_max_size/4, 3*(i + 1), 3./4*x_max_size);
                box(forks[i].window, 0, 0);
                forks[i].in_use = false;


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


    end_screen();

    return 0;
}