#include "Fork.cpp"
 
extern std::mutex refresh_mtx;
extern bool running;

class Philosopher
{
    public:
    int chairId, window_width, cell_width, currentTime, baseTime = 4000, philsNmb;//time in miliseconds
    float percent;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point now;
    std::chrono::duration<double> diff;
    // enum State{ think, hungry, eat};
    // State state;
    std::string state, leftFork, rightFork;
    std::vector<Fork>& forks;
    WINDOW* statusWin;    
    WINDOW* progresWin;    
    WINDOW* forksWin;    

    public:
    Philosopher(std::vector<Fork>& _forks, int _chairId, int _window_width, int _philsNmb) : forks(_forks), chairId(_chairId), 
                                                                                            window_width(_window_width), philsNmb(_philsNmb)
    {
        leftFork = "None ";
        rightFork = "None";
        cell_width = window_width/4;
        statusWin = newwin(3, cell_width, 3*(chairId + 1), 0);
        progresWin = newwin(3, cell_width, 3*(chairId + 1), window_width/4);
        forksWin = newwin(3, cell_width, 3*(chairId + 1), window_width/2);
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
            //statusWin
            werase(statusWin);
            box(statusWin, 0, 0);
            wmove(statusWin, 1, 1);
            wprintw(statusWin, "Philosopher nr %d ", chairId);
            wprintw(statusWin, "THINKING");
            wrefresh(statusWin);
            //progresWin
            werase(progresWin);
            box(progresWin, 0, 0);
            wmove(progresWin, 1, 1);
            wrefresh(progresWin);
            //forksWin
            werase(forksWin);
            box(forksWin, 0, 0);
            wmove(forksWin, 1, 1);
            wprintw(forksWin, "Used forks: ");
            wprintw(forksWin, leftFork.c_str());
            wprintw(forksWin, rightFork.c_str());
            wrefresh(forksWin);
        }
        currentTime = baseTime + rand()%1001;
        currentTime = currentTime / (cell_width-2);
        for(int i = 1; i <= cell_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(currentTime));
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                mvwprintw(progresWin, 1, i, "=");
                wrefresh(progresWin);
            }

        }

        {
            std::lock_guard<std::mutex>refresh_guard(refresh_mtx);
            werase(statusWin);
            box(statusWin, 0, 0);
            wmove(statusWin, 1, 1);
            wprintw(statusWin, "Philosopher nr %d ", chairId);
            wprintw(statusWin, "WAITING");
            wrefresh(statusWin);
        }
    }

    void get_forks(){
        std::unique_lock<std::mutex> ul(forks[chairId].mtx);
        bool temp;
        //first get fork with lower id 
        if(chairId < (chairId+1)%philsNmb){
            //left fork
            forks[chairId].cv.wait(ul, [&temp = forks[chairId].free]{return temp;});
            forks[chairId].free = false;
            leftFork = std::to_string(chairId) + " ";
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                werase(forksWin);
                box(forksWin, 0, 0);
                wmove(forksWin, 1, 1);
                wprintw(forksWin, "Used forks: ");
                wprintw(forksWin, leftFork.c_str());
                wprintw(forksWin, rightFork.c_str());
                wrefresh(forksWin);
            }
            //right fork
            forks[chairId+1].cv.wait(ul, [&temp = forks[chairId+1].free]{return temp;});
            forks[chairId+1].free = false;
            rightFork = std::to_string(chairId+1) + " ";
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                werase(forksWin);
                box(forksWin, 0, 0);
                wmove(forksWin, 1, 1);
                wprintw(forksWin, "Used forks: ");
                wprintw(forksWin, leftFork.c_str());
                wprintw(forksWin, rightFork.c_str());
                wrefresh(forksWin);
            }
        }
        else{
            //right fork
            forks[chairId+1].cv.wait(ul, [&temp = forks[chairId+1].free]{return temp;});
            forks[chairId+1].free = false;
            rightFork = std::to_string(chairId+1) + " ";
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                werase(forksWin);
                box(forksWin, 0, 0);
                wmove(forksWin, 1, 1);
                wprintw(forksWin, "Used forks: ");
                wprintw(forksWin, leftFork.c_str());
                wprintw(forksWin, rightFork.c_str());
                wrefresh(forksWin);
            }
            //left fork
            forks[chairId].cv.wait(ul, [&temp = forks[chairId].free]{return temp;});
            forks[chairId].free = false;
            leftFork = std::to_string(chairId) + " ";
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                werase(forksWin);
                box(forksWin, 0, 0);
                wmove(forksWin, 1, 1);
                wprintw(forksWin, "Used forks: ");
                wprintw(forksWin, leftFork.c_str());
                wprintw(forksWin, rightFork.c_str());
                wrefresh(forksWin);
            }
        }
    }

    void eating(){
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            //statusWin
            werase(statusWin);
            box(statusWin, 0, 0);
            wmove(statusWin, 1, 1);
            wprintw(statusWin, "Philosopher nr %d ", chairId);
            wprintw(statusWin, "EATING");
            wrefresh(statusWin);
            //progresWin
            werase(progresWin);
            box(progresWin, 0, 0);
            wmove(progresWin, 1, 1);
            wrefresh(progresWin);
        }
        currentTime = baseTime + rand()%1001;
        currentTime = currentTime / (cell_width-2);
        for(int i = 1; i <= cell_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(currentTime));
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                mvwprintw(progresWin, 1, i, "#");
                wrefresh(progresWin);
            }

        }


    }

    void put_forks(){
        std::unique_lock<std::mutex> ul(forks[chairId].mtx);
        leftFork = "None ";
        rightFork = "None";
        forks[chairId+1].free = true;
        forks[chairId].free = true;
        werase(forksWin);
        box(forksWin, 0, 0);
        wmove(forksWin, 1, 1);
        wprintw(forksWin, "Used forks: ");
        wprintw(forksWin, leftFork.c_str());
        wprintw(forksWin, rightFork.c_str());
        wrefresh(forksWin);
        forks[chairId+1].cv.notify_one();
        forks[chairId].cv.notify_one();
    }

    void feast(){
        while(running){
            thinking();
            get_forks();
            eating();
            put_forks();
        }
    }

};