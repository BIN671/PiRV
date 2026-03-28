#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>
#include <fstream>
#include <atomic>
#include <iomanip>

class Bank {
private:
    std::vector<double> accounts;
    std::vector<std::mutex> account_mutexes;
    std::atomic<double> total_balance;
    std::mutex log_mutex;
    std::ofstream log_file;
    std::mutex transaction_mutex;
    std::condition_variable transaction_cv;
    int remaining_transactions;
    std::atomic<int> active_threads;

public:
    Bank(int num_clients, double initial_balance, int total_transactions)
        : accounts(num_clients, initial_balance),
        account_mutexes(num_clients),
        total_balance(initial_balance* num_clients),
        remaining_transactions(total_transactions),
        active_threads(0) {
        log_file.open("transactions.log");
        if (log_file.is_open()) {
            log_file << "Bank Transaction Log\n";
            log_file << "Start time: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n\n";
        }
    }

    ~Bank() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }

    bool transfer(int from, int to, double amount, int thread_id, int transaction_id) {
        if (from == to) return false;

        if (amount <= 0) return false;

        int first = std::min(from, to);
        int second = std::max(from, to);

        std::unique_lock<std::mutex> lock1(account_mutexes[first]);
        std::unique_lock<std::mutex> lock2(account_mutexes[second]);

        if (accounts[from] >= amount) {
            accounts[from] -= amount;
            accounts[to] += amount;

            double old_total = total_balance.load();
            while (!total_balance.compare_exchange_weak(old_total, old_total));

            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

            std::lock_guard<std::mutex> log_lock(log_mutex);
            if (log_file.is_open()) {
                log_file << "[" << now_ms << "] Thread " << thread_id
                    << " Transaction " << transaction_id
                    << " From: " << from << " To: " << to
                    << " Amount: " << std::fixed << std::setprecision(2) << amount
                    << " Balances: [" << accounts[first] << ", " << accounts[second] << "]\n";
            }

            return true;
        }

        return false;
    }

    void log_initial_state() {
        std::lock_guard<std::mutex> log_lock(log_mutex);
        if (log_file.is_open()) {
            log_file << "\nInitial Account Balances\n";
            for (size_t i = 0; i < accounts.size(); ++i) {
                log_file << "Client " << i << ": " << std::fixed << std::setprecision(2) << accounts[i] << "\n";
            }
            log_file << "Total Bank Balance: " << total_balance.load() << "\n\n";
        }
    }

    void log_final_state() {
        std::lock_guard<std::mutex> log_lock(log_mutex);
        if (log_file.is_open()) {
            log_file << "\nFinal Account Balances\n";
            for (size_t i = 0; i < accounts.size(); ++i) {
                log_file << "Client " << i << ": " << std::fixed << std::setprecision(2) << accounts[i] << "\n";
            }
            log_file << "Total Bank Balance: " << total_balance.load() << "\n";
            log_file << "\nEnd of Transaction Log\n";
        }
    }

    bool get_transaction() {
        std::unique_lock<std::mutex> lock(transaction_mutex);
        if (remaining_transactions > 0) {
            remaining_transactions--;
            return true;
        }
        return false;
    }

    void increment_active() {
        active_threads++;
    }

    void decrement_active() {
        active_threads--;
        transaction_cv.notify_one();
    }

    void wait_for_completion() {
        std::unique_lock<std::mutex> lock(transaction_mutex);
        transaction_cv.wait(lock, [this]() {
            return active_threads.load() == 0 && remaining_transactions == 0;
            });
    }

    double get_balance(int client) {
        std::lock_guard<std::mutex> lock(account_mutexes[client]);
        return accounts[client];
    }

    double get_total_balance() {
        return total_balance.load();
    }

    size_t num_clients() const {
        return accounts.size();
    }
};

class ClientTransaction {
private:
    Bank& bank;
    int thread_id;
    std::mt19937 rng;
    std::uniform_int_distribution<int> client_dist;
    std::uniform_real_distribution<double> amount_dist;
    int transaction_counter;

public:
    ClientTransaction(Bank& b, int tid, int num_clients, unsigned int seed)
        : bank(b), thread_id(tid), rng(seed),
        client_dist(0, num_clients - 1),
        amount_dist(10.0, 500.0),
        transaction_counter(0) {}

    void process_transactions() {
        bank.increment_active();

        while (true) {
            if (!bank.get_transaction()) {
                break;
            }

            int from = client_dist(rng);
            int to = client_dist(rng);
            double amount = amount_dist(rng);

            transaction_counter++;

            bool success = bank.transfer(from, to, amount, thread_id, transaction_counter);

            if (!success) {
                bank.get_transaction();
            }
        }

        bank.decrement_active();
    }
};

int main() {
    const int NUM_CLIENTS = 10;
    const double INITIAL_BALANCE = 1000.0;
    const int NUM_THREADS = 5;
    const int TOTAL_TRANSACTIONS = 100;

    Bank bank(NUM_CLIENTS, INITIAL_BALANCE, TOTAL_TRANSACTIONS);

    bank.log_initial_state();

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    std::random_device rd;

    for (int i = 0; i < NUM_THREADS; ++i) {
        unsigned int seed = rd();
        threads.emplace_back([&bank, i, NUM_CLIENTS, seed]() {
            ClientTransaction client(bank, i, NUM_CLIENTS, seed);
            client.process_transactions();
            });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Number of clients: " << NUM_CLIENTS << "\n";
    std::cout << "Initial balance per client: " << INITIAL_BALANCE << "\n";
    std::cout << "Number of threads: " << NUM_THREADS << "\n";
    std::cout << "Total transactions processed: " << TOTAL_TRANSACTIONS << "\n\n";

    std::cout << "Final Account Balances:\n";
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        std::cout << "Client " << i << ": "
            << std::fixed << std::setprecision(2)
            << bank.get_balance(i) << "\n";
    }

    std::cout << "\nTotal Bank Balance: "
        << std::fixed << std::setprecision(2)
        << bank.get_total_balance() << "\n";

    std::cout << "\nExecution Time: " << duration.count() << " ms\n";

    bank.log_final_state();

    std::cout << "\nTransaction loged\n";

    return 0;
}