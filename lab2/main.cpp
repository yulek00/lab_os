#include "threads.h"
#include "exceptions.h"

#include <iostream>
#include <vector>
#include <iomanip> 
#include <cmath>   
#include <stdexcept>
#include <chrono>  
#include <random>  
#include <string>

struct GlobalResult {
    long long total_hits = 0;  
    pthread_mutex_t mutex;      
};

struct ThreadArgs {
    double radius;            
    long long points_to_generate; 
    GlobalResult* global_res;  
};

void* calculate_area_chunk(void* args) {
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);

    double R = thread_args->radius;
    long long N = thread_args->points_to_generate;
    double R_squared = R * R;
    long long local_hits = 0;
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distrib(-R, R);

    for (long long i = 0; i < N; ++i) {
        double x = distrib(generator);
        double y = distrib(generator);

        if ((x * x + y * y) <= R_squared) {
            local_hits++;
        }
    }

    GlobalResult* global_res = thread_args->global_res;

    pthread_mutex_lock(&global_res->mutex);
    global_res->total_hits += local_hits;
    pthread_mutex_unlock(&global_res->mutex);

    return nullptr;
}

int main(int argc, char* argv[]) {
    double radius;
    int max_threads;
    const long long TOTAL_POINTS = 1000000000LL;

    try {
        if (argc != 3) {
            throw exceptions::ArgumentException("Incorrect number of arguments.");
        }

        radius = std::stod(argv[1]);
        max_threads = std::stoi(argv[2]);

        if (radius <= 0 || max_threads <= 0) {
            throw exceptions::ArgumentException("Radius and Max_Threads must be positive numbers.");
        }
    }

    catch (const std::exception& e) { 
        std::cerr << "Startup Error: " << e.what() << std::endl;
        std::cerr << "Usage: " << argv[0] << " <Radius> <Max_Threads>" << std::endl;
        return 1;
    }

    long long points_per_thread = TOTAL_POINTS / max_threads;

    GlobalResult global_res;
    if (pthread_mutex_init(&global_res.mutex, nullptr) != 0) {
        std::cerr << "Fatal error: Failed to initialize mutex." << std::endl;
        return 1;
    }

    std::vector<thread::Thread> thread_pool;
    std::vector<ThreadArgs> args_data(max_threads); 

    for (int i = 0; i < max_threads; ++i) {
        args_data[i].radius = radius;
        args_data[i].points_to_generate = points_per_thread;
        args_data[i].global_res = &global_res;
        
        thread_pool.emplace_back(calculate_area_chunk);
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();

    try {
        for (int i = 0; i < max_threads; ++i) {
            thread_pool[i].Run(&args_data[i]);
        }

        for (int i = 0; i < max_threads; ++i) {
            thread_pool[i].Join();
        }
    }
    catch (const exceptions::ThreadException& e) {
        std::cerr << "Critical Error (Create): " << e.what() << std::endl;
        pthread_mutex_destroy(&global_res.mutex);
        return 1;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double execution_time = std::chrono::duration<double>(end_time - start_time).count();

    long long total_hits = global_res.total_hits;
    double area_square = 4.0 * radius * radius;
    
    double estimated_area = area_square * (static_cast<double>(total_hits) / TOTAL_POINTS);
    double analytical_area = M_PI * radius * radius;

    std::cout << "\n--- Monte Carlo Simulation Results ---" << std::endl;
    std::cout << "Threads Used:      " << max_threads << std::endl;
    std::cout << "Total Points:      " << TOTAL_POINTS << std::endl;
    std::cout << "Total Hits:        " << total_hits << std::endl;
    std::cout << "Estimated Area:    " << std::fixed << std::setprecision(8) << estimated_area << std::endl;
    std::cout << "Analytical Area:   " << std::fixed << std::setprecision(8) << analytical_area << std::endl;
    std::cout << "Execution Time:    " << std::fixed << std::setprecision(6) << execution_time << " seconds" << std::endl;

    pthread_mutex_destroy(&global_res.mutex);

    return 0;
}