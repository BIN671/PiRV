#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <random>

std::mutex mtx;
std::condition_variable assembly_cv;
std::condition_variable painting_cv;
std::condition_variable packaging_cv;

std::atomic<int> cars_assembled{ 0 };
std::atomic<int> cars_painted{ 0 };
std::atomic<int> cars_packaged{ 0 };

const int TOTAL_CARS = 10;
int current_car = 1;

void assembly_stage(int operator_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(500, 1000);

    while (cars_assembled < TOTAL_CARS) {
        std::unique_lock<std::mutex> lock(mtx);
        assembly_cv.wait(lock, [] {
            return true;
            });

        if (cars_assembled >= TOTAL_CARS) {
            break;
        }

        int car_id = current_car;
        current_car++;
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        std::cout << "Operator " << operator_id << " assembled car " << car_id << "\n";

        cars_assembled++;
        painting_cv.notify_one();
    }
}

void painting_stage(int operator_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(300, 800);

    int painted_count = 0;

    while (cars_painted < TOTAL_CARS) {
        std::unique_lock<std::mutex> lock(mtx);
        painting_cv.wait(lock, [&] {
            return cars_assembled > painted_count;
            });

        if (cars_painted >= TOTAL_CARS) {
            break;
        }

        int car_id = painted_count + 1;
        painted_count++;
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        std::cout << "Operator " << operator_id << " painted car " << car_id << "\n";

        cars_painted++;
        packaging_cv.notify_one();
    }
}

void packaging_stage(int operator_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(200, 600);

    int packaged_count = 0;

    while (cars_packaged < TOTAL_CARS) {
        std::unique_lock<std::mutex> lock(mtx);
        packaging_cv.wait(lock, [&] {
            return cars_painted > packaged_count;
            });

        if (cars_packaged >= TOTAL_CARS) {
            break;
        }

        int car_id = packaged_count + 1;
        packaged_count++;
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        std::cout << "Operator " << operator_id << " packaged car " << car_id << "\n";

        cars_packaged++;
    }
}

int main() {
    std::vector<std::thread> assembly_operators;
    std::vector<std::thread> painting_operators;
    std::vector<std::thread> packaging_operators;

    const int NUM_ASSEMBLY = 2;
    const int NUM_PAINTING = 2;
    const int NUM_PACKAGING = 2;

    for (int i = 0; i < NUM_ASSEMBLY; i++) {
        assembly_operators.emplace_back(assembly_stage, i + 1);
    }

    for (int i = 0; i < NUM_PAINTING; i++) {
        painting_operators.emplace_back(painting_stage, i + 1);
    }

    for (int i = 0; i < NUM_PACKAGING; i++) {
        packaging_operators.emplace_back(packaging_stage, i + 1);
    }

    for (int i = 0; i < TOTAL_CARS; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        assembly_cv.notify_one();
    }

    for (auto& t : assembly_operators) {
        t.join();
    }

    for (auto& t : painting_operators) {
        t.join();
    }

    for (auto& t : packaging_operators) {
        t.join();
    }

    std::cout << "\nProduction completed!\n";
    std::cout << "Cars assembled: " << cars_assembled << "\n";
    std::cout << "Cars painted: " << cars_painted << "\n";
    std::cout << "Cars packaged: " << cars_packaged << "\n";

    return 0;
}