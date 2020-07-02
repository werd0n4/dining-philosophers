#include "Fork.cpp"
 
extern std::mutex refresh_mtx;
extern bool running;

enum Colors{think, eat, wait, frame};

class Philosopher
{
    public:
    int chairId, window_width, cell_width, currentTime, baseTime = 4000, philsNmb;//time in miliseconds
    int firstFork, secondFork;
    float percent;
    std::string state, leftFork, rightFork;
    std::vector<Fork>& forks;
    WINDOW* statusWin;    
    WINDOW* progresWin;    
    WINDOW* forksWin;    

    public:
    Philosopher(std::vector<Fork>& _forks, int _chairId, int _window_width, int _philsNmb) : forks(_forks), chairId(_chairId), 
                                                                                            window_width(_window_width), philsNmb(_philsNmb)
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
        leftFork = "None";
        rightFork = "None";
        cell_width = window_width/3;
        statusWin = newwin(3, cell_width, 3*(chairId + 1), 0);
        progresWin = newwin(3, cell_width, 3*(chairId + 1), window_width/3);
        forksWin = newwin(3, cell_width, 3*(chairId + 1), 2./3*window_width);
        box(statusWin, 0, 0);
        box(progresWin, 0, 0);
        box(forksWin, 0, 0);

        wrefresh(statusWin);
        wrefresh(progresWin);
        wrefresh(forksWin);
    }

    void thinking(){
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            refresh_statusWin("THINKING");
            clear_progresWin();
            update_fork('L', -1);
            update_fork('R', -1);
            // refresh_forksWin();
        }
        currentTime = baseTime + rand()%1001;
        currentTime = currentTime / (cell_width-2);
        for(int i = 1; i <= cell_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(currentTime));
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                wattron(progresWin, COLOR_PAIR(1));
                mvwprintw(progresWin, 1, i, "=");
                wattroff(progresWin, COLOR_PAIR(1));
                wrefresh(progresWin);
            }
        }

        {
            std::lock_guard<std::mutex>refresh_guard(refresh_mtx);
            refresh_statusWin("WAITING");
        }
    }

    void get_forks(){
        std::unique_lock<std::mutex> ul(forks[chairId].mtx);
        bool temp;

        //first fork
        forks[firstFork].cv.wait(ul, [&temp = forks[firstFork].free]{return temp;});
        forks[firstFork].free = false;
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            update_fork('R', firstFork);
        }
        //second fork
        forks[secondFork].cv.wait(ul, [&temp = forks[secondFork].free]{return temp;});
        forks[secondFork].free = false;
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            update_fork('L', secondFork);
        }
    }

    void eating(){
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            refresh_statusWin("EATING");
            clear_progresWin();
        }
        currentTime = baseTime + rand()%1001;
        currentTime = currentTime / (cell_width-2);//minus 2 for cell frame
        for(int i = 1; i <= cell_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(currentTime));
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                wattron(progresWin, COLOR_PAIR(2));
                mvwprintw(progresWin, 1, i, "#");
                wattroff(progresWin, COLOR_PAIR(2));
                wrefresh(progresWin);
            }
        }
    }

    void put_forks(){
        {
            std::unique_lock<std::mutex> ul(forks[firstFork].mtx);
            leftFork = "None";
            rightFork = "None";
            forks[secondFork].free = true;
            forks[firstFork].free = true;
            // refresh_forksWin();
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                update_fork('L',-1);
                update_fork('R',-1);
            }
        }
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
        int color = 5;
        if(leftFork !="None" && rightFork !="None"){
            color = 2;
        }
        werase(forksWin);
        wattron(forksWin, COLOR_PAIR(color));
        box(forksWin, 0, 0);
        wattroff(forksWin, COLOR_PAIR(color));
        wmove(forksWin, 1, 1);
        wprintw(forksWin, "Used forks: ");
        wprintw(forksWin, (leftFork+" ").c_str());
        wprintw(forksWin, rightFork.c_str());
        wrefresh(forksWin);
    }

    void clear_progresWin(){
        werase(progresWin);
        box(progresWin, 0, 0);
        wmove(progresWin, 1, 1);
        wrefresh(progresWin);
    }

    void update_fork(char side, int forkId){//forkId == -1 if philosopher puts fork down
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

    void refresh_statusWin(std::string state){
        int color;
        if(state == "THINKING"){
            color = 1;
        } else if(state == "EATING"){
            color = 2;
        } else{//WAITING
            color = 3;
        }

        werase(statusWin);
        wattron(statusWin, COLOR_PAIR(color));
        box(statusWin, 0, 0);
        wattroff(statusWin, COLOR_PAIR(color));
        wmove(statusWin, 1, 1);
        wprintw(statusWin, "Philosopher nr %d ", chairId);
        wprintw(statusWin, state.c_str());
        wrefresh(statusWin);
    }
};