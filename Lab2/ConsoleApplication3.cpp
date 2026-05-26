#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <atomic>
#include <string>
#include <sstream>

using namespace std;
using namespace std::chrono_literals;

class CountingSemaphore {
public:
    explicit CountingSemaphore(int max_count) : max_count_(max_count), current_count_(max_count) {}
    void acquire() {
        unique_lock<mutex> lock(mutex_);
        cond_.wait(lock, [this] { return current_count_ > 0; });
        --current_count_;
    }
    void release() {
        unique_lock<mutex> lock(mutex_);
        if (current_count_ < max_count_) {
            ++current_count_;
            cond_.notify_one();
        }
    }
private:
    const int max_count_;
    int current_count_;
    mutex mutex_;
    condition_variable cond_;
};

struct DataPacket {
    int station_id;
    int priority;   
    int value;
    bool critical;
    chrono::steady_clock::time_point timestamp;
};

struct ComparePacket {
    bool operator()(const DataPacket& a, const DataPacket& b) {
        if (a.critical != b.critical) return a.critical < b.critical;
        if (a.priority != b.priority) return a.priority > b.priority;
        return a.timestamp > b.timestamp;
    }
};

const int INIT_HANDLERS = 2;
const int MAX_HANDLERS = 6;
const double LOAD_THRESHOLD = 0.8;

atomic<int> active_handlers(INIT_HANDLERS);
CountingSemaphore handler_sem(INIT_HANDLERS);
priority_queue<DataPacket, vector<DataPacket>, ComparePacket> data_queue;
mutex queue_mtx, cout_mtx;
atomic<bool> emergency(false);
atomic<bool> stop_flag(false);

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> prio_dist(1, 3);
uniform_int_distribution<> val_dist(50, 150);

string to_str(int n) {
    ostringstream oss;
    oss << n;
    return oss.str();
}

void print(const string& s) {
    lock_guard<mutex> lock(cout_mtx);
    cout << s << endl;
}

void load_monitor() {
    while (!stop_flag) {
        this_thread::sleep_for(chrono::seconds(2));
        size_t qsize;
        {
            lock_guard<mutex> lock(queue_mtx);
            qsize = data_queue.size();
        }
        int curr = active_handlers.load();
        double load = (double)qsize / (curr * 2);
        if (load > LOAD_THRESHOLD && curr < MAX_HANDLERS) {
            active_handlers++;
            handler_sem.release();  
            print("[MON] load=" + to_str(load) + " -> +handler, total=" + to_str(active_handlers));
        }
    }
}

void data_handler(int id) {
    while (!stop_flag) {
        handler_sem.acquire();
        if (stop_flag) { handler_sem.release(); break; }

        DataPacket p;
        bool has = false;
        {
            lock_guard<mutex> lock(queue_mtx);
            if (!data_queue.empty()) {
                p = data_queue.top();
                data_queue.pop();
                has = true;
            }
        }
        if (!has) {
            handler_sem.release();
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }

        print("[H" + to_str(id) + "] proc station " + to_str(p.station_id) + " prio=" + to_str(p.priority) + " crit=" + (p.critical ? "yes" : "no"));
        this_thread::sleep_for(chrono::milliseconds(500 + p.priority * 200));
        print("[H" + to_str(id) + "] done");
        handler_sem.release();
    }
}

void station(int sid) {
    mt19937 st_gen(random_device{}() + sid);
    uniform_int_distribution<> local_interval(800, 2500);
    while (!stop_flag) {
        this_thread::sleep_for(chrono::milliseconds(local_interval(st_gen)));
        int prio = prio_dist(st_gen);
        int val = val_dist(st_gen);
        bool crit = (st_gen() % 10 == 0);
        if (emergency) {
            crit = true;
            prio = max(1, prio - 1);
        }
        if (emergency && prio > 2) {
            print("[ST" + to_str(sid) + "] emergency: drop low-prio packet");
            continue;
        }
        DataPacket p{ sid, prio, val, crit, chrono::steady_clock::now() };
        {
            lock_guard<mutex> lock(queue_mtx);
            data_queue.push(p);
        }
        print("[ST" + to_str(sid) + "] sent prio=" + to_str(prio) + " crit=" + (crit ? "yes" : "no"));
    }
}

void emergency_trigger() {
    this_thread::sleep_for(chrono::seconds(15));
    print("\n*** EMERGENCY ACTIVATED ***");
    emergency = true;
    this_thread::sleep_for(chrono::seconds(10));
    emergency = false;
    print("*** EMERGENCY DEACTIVATED ***");
}

int main() {
    print("=== Adaptive Energy Monitoring ===");
    vector<thread> handlers;
    for (int i = 0; i < INIT_HANDLERS; ++i) handlers.emplace_back(data_handler, i);
    vector<thread> stations;
    for (int i = 1; i <= 10; ++i) stations.emplace_back(station, i);
    thread monitor(load_monitor);
    thread emerg(emergency_trigger);

    this_thread::sleep_for(chrono::seconds(40));
    stop_flag = true;
    emerg.join();
    monitor.join();
    for (auto& t : stations) t.join();
    for (auto& t : handlers) t.join();
    print("System stopped");
    return 0;
}