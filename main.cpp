#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <memory>

using namespace std;

vector<unique_ptr<mutex>> forks; // Wektor widelców
mutex cout_mutex; // Mutex do synchronizacji wypisywania

void philosopher(int id) {
    int left_fork = id;                // Left fork index
    int right_fork = (id + 1) % forks.size(); // Right fork index (circular table)

    while (true) {
        // Thinking
        {
            lock_guard<mutex> lock(cout_mutex);
            cout << string(id * 2, ' ') << "P" << id << ": thinking" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));

        // Hungry
        {
            lock_guard<mutex> lock(cout_mutex);
            cout << "Philosopher " << id << " is hungry" << endl;
        }

        // Pick up forks in order (lower index first to avoid deadlock)
        if (left_fork < right_fork) {
            forks[left_fork]->lock();
            forks[right_fork]->lock();
        } else {
            forks[right_fork]->lock();
            forks[left_fork]->lock();
        }

        // Eating
        {
            lock_guard<mutex> lock(cout_mutex);
            cout << "Philosopher " << id << " is eating" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));

        // Put down forks
        forks[left_fork]->unlock();
        forks[right_fork]->unlock();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <number_of_philosophers>" << endl;
        return 1;
    }

    int num_philosophers = atoi(argv[1]);
    if (num_philosophers < 2) {
        cout << "Number of philosophers must be at least 2" << endl;
        return 1;
    }

    // Inicjalizacja widelców
    for (int i = 0; i < num_philosophers; i++) {
        forks.emplace_back(make_unique<mutex>());
    }

    // Tworzenie wątków filozofów
    vector<thread> philosophers;
    for (int i = 0; i < num_philosophers; i++) {
        philosophers.emplace_back(philosopher, i);
    }

    // Czekanie na działanie symulacji
    this_thread::sleep_for(chrono::seconds(10));

    // Odłączanie wątków
    for (auto& t : philosophers) {
        t.detach();
    }

    cout << "Simulation ended" << endl;
    return 0;
}