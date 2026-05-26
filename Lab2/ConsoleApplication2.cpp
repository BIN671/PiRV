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


struct Task {
    int id;
    int priority;   // 1 – highest
    int duration;   // seconds
    bool critical;
};

struct CompareTask {
    bool operator()(const Task& a, const Task& b) {
        if (a.critical != b.critical) return a.critical < b.critical;
        return a.priority > b.priority;
    }
};

const int NUM_PROCESSORS = 4;
const int NUM_WORKERS = 10;
CountingSemaphore processor_sem(NUM_PROCESSORS);
priority_queue<Task, vector<Task>, CompareTask> task_queue;
mutex queue_mtx;
mutex cout_mtx;
atomic<bool> stop_flag(false);

atomic<bool> processor_broken[NUM_PROCESSORS];
mutex proc_mtx[NUM_PROCESSORS];
condition_variable proc_cv[NUM_PROCESSORS];

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dur_dist(1, 5);
uniform_int_distribution<> prio_dist(1, 3);

string to_str(int n) {
    ostringstream oss;
    oss << n;
    return oss.str();
}

void print(const string& s) {
    lock_guard<mutex> lock(cout_mtx);
    cout << s << endl;
}

void split_task(const Task& t) {
    if (t.duration <= 2) return;
    int dur1 = t.duration / 2;
    int dur2 = t.duration - dur1;
    Task t1 = { t.id * 100, t.priority, dur1, t.critical };
    Task t2 = { t.id * 100 + 1, t.priority, dur2, t.critical };
    lock_guard<mutex> lock(queue_mtx);
    task_queue.push(t1);
    task_queue.push(t2);
    print("[SPLIT] " + to_str(t.id) + " -> " + to_str(t1.id) + " (" + to_str(dur1) + "s) and " + to_str(t2.id));
}

void add_task(int id, int priority, bool critical, int duration = -1) {
    if (duration == -1) duration = dur_dist(gen);
    Task t{ id, priority, duration, critical };
    {
        lock_guard<mutex> lock(queue_mtx);
        if (task_queue.size() > 5 && t.duration > 2) {
            split_task(t);
            return;
        }
        task_queue.push(t);
    }
    print("[ADD] Task " + to_str(id) + " | prio=" + to_str(priority) + " | dur=" + to_str(duration) + "s | crit=" + (critical ? "yes" : "no"));
}

void worker(int wid, int proc_id) {
    while (!stop_flag) {
        {
            unique_lock<mutex> lock(proc_mtx[proc_id]);
            proc_cv[proc_id].wait(lock, [&] { return !processor_broken[proc_id] || stop_flag; });
        }
        if (stop_flag) break;

        processor_sem.acquire();

        Task task;
        bool has_task = false;
        {
            lock_guard<mutex> lock(queue_mtx);
            if (!task_queue.empty()) {
                task = task_queue.top();
                task_queue.pop();
                has_task = true;
            }
        }
        if (!has_task) {
            processor_sem.release();
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }

        print("[P" + to_str(proc_id + 1) + "][W" + to_str(wid) + "] start task " + to_str(task.id) + " (" + to_str(task.duration) + "s)");

        bool interrupted = false;
        for (int sec = 0; sec < task.duration && !interrupted; ++sec) {
            this_thread::sleep_for(chrono::seconds(1));
            if (processor_broken[proc_id]) interrupted = true;
        }
        if (!interrupted && !processor_broken[proc_id]) {
            print("[P" + to_str(proc_id + 1) + "] task " + to_str(task.id) + " DONE");
        }
        else {
            print("[P" + to_str(proc_id + 1) + "] FAIL! task " + to_str(task.id) + " re-queued");
            lock_guard<mutex> lock(queue_mtx);
            task_queue.push(task);
        }
        processor_sem.release();
    }
}

void failure_monitor() {
    mt19937 rng(random_device{}());
    uniform_int_distribution<> proc_dist(0, NUM_PROCESSORS - 1);
    uniform_int_distribution<> interval_dist(5, 15);
    while (!stop_flag) {
        this_thread::sleep_for(chrono::seconds(interval_dist(rng)));
        int p = proc_dist(rng);
        if (!processor_broken[p]) {
            processor_broken[p] = true;
            print("*** PROCESSOR " + to_str(p + 1) + " FAILED ***");
            this_thread::sleep_for(chrono::seconds(3));
            processor_broken[p] = false;
            print("*** PROCESSOR " + to_str(p + 1) + " RESTORED ***");
            proc_cv[p].notify_all();
        }
    }
}

void task_generator() {
    int id = 1;
    while (!stop_flag) {
        this_thread::sleep_for(chrono::seconds(2));
        bool crit = (id % 5 == 0);
        int prio = crit ? 1 : prio_dist(gen);
        add_task(id++, prio, crit);
    }
}

int main() {
    for (int i = 0; i < NUM_PROCESSORS; ++i) processor_broken[i] = false;

    vector<thread> workers;
    for (int i = 0; i < NUM_WORKERS; ++i) workers.emplace_back(worker, i, i % NUM_PROCESSORS);

    thread monitor(failure_monitor);
    thread generator(task_generator);

    this_thread::sleep_for(chrono::seconds(40));
    stop_flag = true;

    generator.join();
    monitor.join();
    for (auto& t : workers) t.join();
    cout << "System stopped." << endl;
    return 0;
}