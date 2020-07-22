#include "Fork.cpp"

extern std::mutex refresh_mtx;
extern bool running;

enum Colors{ green = 1, red, yellow, cyan, white };

class Philosopher
{
    public:
    int chairId, window_width, cell_width, currentTime, philsNmb;
    const int baseTime = 4000;//time in miliseconds
    int firstFork, secondFork;
    std::string state, leftFork, rightFork;
    std::vector<Fork>& forks;
    WINDOW *statusWin, *progresWin, *forksWin;    

    public:
    Philosopher(std::vector<Fork>& _forks, int _chairId, int _philsNmb) : forks(_forks), chairId(_chairId), philsNmb(_philsNmb)
    {
        //all philosophers except the last one, first take fork from right side
        //the last one takes first fork from left side
        if(chairId == (philsNmb -1)){
            secondFork = chairId;
            firstFork = 0;
        } else{
            firstFork = chairId;
            secondFork = chairId + 1;
        }
        
        // getmaxyx(stdscr, std::ignore, window_width);
        window_width = getmaxx(stdscr);
        leftFork = "None";
        rightFork = "None";
        cell_width = window_width/3;

        statusWin = newwin(3, cell_width, 3*(chairId + 1), 0);
        progresWin = newwin(3, cell_width, 3*(chairId + 1), window_width/3);
        forksWin = newwin(3, cell_width, 3*(chairId + 1), 2./3*window_width);
        box(statusWin, 0, 0);
        box(progresWin, 0, 0);
        box(forksWin, 0, 0);

        const std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
        wrefresh(statusWin);
        wrefresh(progresWin);
        wrefresh(forksWin);
    }

    void thinking(){
        refresh_statusWin("THINKING");
        clear_progresWin();
        update_fork('L', -1);
        update_fork('R', -1);

        currentTime = baseTime + rand()%1001;
        currentTime = currentTime / (cell_width-2);
        for(int i = 1; i <= cell_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(currentTime));
            refresh_progress_bar(i, Colors::green, "=");
        }

        refresh_statusWin("WAITING");
    }

    void get_forks(){
        std::unique_lock<std::mutex> ul(forks[chairId].mtx);

        //first fork
        forks[firstFork].cv.wait(ul, [this]{return forks[firstFork].free.load();});
        forks[firstFork].free.store(false);
        update_fork('R', firstFork);

        //second fork
        forks[secondFork].cv.wait(ul, [this]{return forks[secondFork].free.load();});
        forks[secondFork].free.store(false);
        update_fork('L', secondFork);
    }

    void eating(){
        currentTime = baseTime + rand()%1001;
        currentTime = currentTime / (cell_width-2);//minus 2 for cell frame

        refresh_statusWin("EATING");
        clear_progresWin();

        for(int i = 1; i <= cell_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(currentTime));
            refresh_progress_bar(i, Colors::red, "#");
        }
    }

    void put_forks(){
        leftFork = "None";
        rightFork = "None";
        forks[secondFork].free.store(true);
        forks[firstFork].free.store(true);
        update_fork('L',-1);
        update_fork('R',-1);

        forks[secondFork].cv.notify_one();
        forks[firstFork].cv.notify_one();
    }

    void feast(){
        while(running){
            thinking();
            get_forks();
            eating();
            put_forks();
        }
    }

    void refresh_forksWin(){
        Colors color = white;
        if(leftFork !="None" && rightFork !="None"){
            color = red;
        }
        
        werase(forksWin);
        wattron(forksWin, COLOR_PAIR(color));
        box(forksWin, 0, 0);
        wattroff(forksWin, COLOR_PAIR(color));
        wmove(forksWin, 1, 1);
        wprintw(forksWin, "Used forks: ");
        wprintw(forksWin, (leftFork+" ").c_str());
        wprintw(forksWin, rightFork.c_str());

        const std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
        wrefresh(forksWin);
    }

    void clear_progresWin(){
        werase(progresWin);
        box(progresWin, 0, 0);
        wmove(progresWin, 1, 1);

        const std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
        wrefresh(progresWin);
    }

    void update_fork(const char& side, const int& forkId){//forkId == -1 if philosopher puts fork down
        if(side == 'L'){
            if(forkId == -1){
                leftFork = "None";
            } else{
                leftFork = std::to_string(forkId);
            }   
        }
        else if(side == 'R'){
            if(forkId == -1){
                rightFork = "None";
            } else{
                rightFork = std::to_string(forkId);
            }   
        }
        refresh_forksWin();
    }

    void refresh_statusWin(const std::string& state){
        const Colors color = state == "THINKING" ? green : (state == "EATING" ? red : yellow );

        werase(statusWin);
        wattron(statusWin, COLOR_PAIR(color));
        box(statusWin, 0, 0);
        wattroff(statusWin, COLOR_PAIR(color));
        wmove(statusWin, 1, 1);
        wprintw(statusWin, "Philosopher nr %d ", chairId);
        wprintw(statusWin, state.c_str());

        const std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
        wrefresh(statusWin);
    }

    void refresh_progress_bar(const int& i, const Colors& color, const std::string& symbol){
        wattron(progresWin, COLOR_PAIR(color));
        mvwprintw(progresWin, 1, i, symbol.c_str());
        wattroff(progresWin, COLOR_PAIR(color));

        const std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
        wrefresh(progresWin);
    }
};