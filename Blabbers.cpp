#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <string>

#define NMAX 100

std::mutex mutexes[NMAX];
std::mutex io_mutex;


void simulate(int n, int i) {
    while (true) {
        mutexes[i].lock();
        for (int j = 0; j < n; j++) {
            if (mutexes[j].try_lock()) {
                {
                    std::lock_guard<std::mutex> lock(io_mutex);
                    std::cout << "Blabber " << i << " is calling blabber " << j << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::seconds(2));
                mutexes[j].unlock();
                break;
            }
        }
        mutexes[i].unlock();

        {
            std::lock_guard<std::mutex> lock(io_mutex);
            std::cout << "Blabber " << i << " is waiting for a call" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


int main(int argc, char *argv[]) {
    const int n = std::stoi(argv[1]);
    if (n <= 0 || n > 100) {
        std::cout << "Количество болтунов должно быть целочисленным числом в интервале (0, 100]"<<std::endl;
        return 0;
    }
    std::thread* sim_threads= new std::thread[n];
    for (int i = 0; i < n; i++) {
        // run blabber simulation thread
        sim_threads[i] = std::thread(simulate, n, i);
    }

    for (int i = 0; i < n; i++) {
        sim_threads[i].join();
    }
    return 0;
}



