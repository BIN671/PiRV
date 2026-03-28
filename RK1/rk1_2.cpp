#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>
#include <atomic>

std::mutex order_mutex;
std::mutex delivery_mutex;
std::condition_variable order_cv;
std::condition_variable delivery_cv;

std::queue<int> pending_orders;
std::queue<int> ready_for_delivery;
std::atomic<int> orders_processed{ 0 };
std::atomic<int> orders_delivered{ 0 };
const int TOTAL_ORDERS = 10;

int next_order_id = 1;

void process_order(int order_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100, 500);

    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
    std::cout << "Order " << order_id << " processed\n";
}

void deliver_order(int order_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100, 300);

    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
    std::cout << "Order " << order_id << " delivered\n";
}

void processing_worker(int worker_id, int max_concurrent) {
    static int active_processing = 0;
    static std::mutex active_mutex;

    while (orders_processed < TOTAL_ORDERS) {
        int order_id = -1;

        {
            std::unique_lock<std::mutex> lock(order_mutex);
            if (pending_orders.empty()) {
                if (orders_processed >= TOTAL_ORDERS) {
                    break;
                }
                continue;
            }
            order_id = pending_orders.front();
            pending_orders.pop();
        }

        {
            std::unique_lock<std::mutex> lock(active_mutex);
            while (active_processing >= max_concurrent) {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                lock.lock();
            }
            active_processing++;
        }

        process_order(order_id);

        {
            std::unique_lock<std::mutex> lock(active_mutex);
            active_processing--;
        }

        {
            std::lock_guard<std::mutex> lock(delivery_mutex);
            ready_for_delivery.push(order_id);
            orders_processed++;
        }
        delivery_cv.notify_one();
    }
}

void delivery_worker(int worker_id, int max_concurrent) {
    static int active_delivery = 0;
    static std::mutex active_mutex;

    while (orders_delivered < TOTAL_ORDERS) {
        int order_id = -1;

        {
            std::unique_lock<std::mutex> lock(delivery_mutex);
            if (ready_for_delivery.empty()) {
                if (orders_delivered >= TOTAL_ORDERS) {
                    break;
                }
                continue;
            }
            order_id = ready_for_delivery.front();
            ready_for_delivery.pop();
        }

        {
            std::unique_lock<std::mutex> lock(active_mutex);
            while (active_delivery >= max_concurrent) {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                lock.lock();
            }
            active_delivery++;
        }

        deliver_order(order_id);

        {
            std::unique_lock<std::mutex> lock(active_mutex);
            active_delivery--;
        }

        orders_delivered++;
    }
}

int main() {
    for (int i = 1; i <= TOTAL_ORDERS; i++) {
        pending_orders.push(i);
    }

    std::vector<std::thread> processing_threads;
    std::vector<std::thread> delivery_threads;

    const int NUM_PROCESSING_STAFF = 3;
    const int NUM_DELIVERY_STAFF = 2;

    for (int i = 0; i < NUM_PROCESSING_STAFF; i++) {
        processing_threads.emplace_back(processing_worker, i, NUM_PROCESSING_STAFF);
    }

    for (int i = 0; i < NUM_DELIVERY_STAFF; i++) {
        delivery_threads.emplace_back(delivery_worker, i, NUM_DELIVERY_STAFF);
    }

    for (auto& t : processing_threads) {
        t.join();
    }

    for (auto& t : delivery_threads) {
        t.join();
    }

    std::cout << "All orders processed and delivered successfully\n";

    return 0;
}