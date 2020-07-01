#include <iostream>
#include <ncurses.h>
#include <thread>
#include <condition_variable>

#include "Philosopher.cpp"
#include "Fork.cpp"

bool running;

WINDOW** init_screen()
{
    initscr();   
    int y_max_size, x_max_size;
    getmaxyx(stdscr, y_max_size, x_max_size);
    WINDOW* inputWin = newwin(3, x_max_size/2, 0, 0);
    WINDOW* exitWin = newwin(3, x_max_size/2, 0, x_max_size/2);
    box(inputWin, 0, 0);
    box(exitWin, 0, 0);
    wmove(inputWin, 1, 1);
    wprintw(inputWin, "How many philosophers will be eating(>=5): ");
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

void refresh_screen(WINDOW** windows)
{
    while(running){
        // refresh();
        // wrefresh(windows[1]);
        // wrefresh(windows[0]);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void user_input(WINDOW** windows, int& philsNmb)
{
    bool isStarted = false;
    char* input;
    while(running){
        if(!isStarted){
            mvwscanw(windows[0], 1, 44,"%d", &philsNmb);
            isStarted = true;
            wmove(windows[1], 1, 65);
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
    int philsNmb;

    running = true;
    WINDOW** windows = init_screen();
    
    std::thread screenRefresh([&windows]{refresh_screen(windows);});
    std::thread userInput([&windows, &philsNmb]{user_input(windows, philsNmb);});

    screenRefresh.join();
    userInput.join();


    end_screen();

    return 0;
}