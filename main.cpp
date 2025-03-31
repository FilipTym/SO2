#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <memory>
#include <iomanip>

using namespace std;

// Global vector of unique_ptr to mutexes representing forks
vector<unique_ptr<mutex>> forks;
// Global mutex to synchronize console output
mutex cout_mutex;

// Function representing a philosopher's behavior
void philosopher(int id) {
    // Define indices for left and right forks
    int left_fork = id;                    // Left fork is the philosopher's own index
    int right_fork = (id + 1) % forks.size(); // Right fork is the next index, wrapping around (circular table)

    while (true) {  // Infinite loop to simulate continuous philosopher activity
        // Thinking state
        {
            lock_guard<mutex> lock(cout_mutex); // Lock cout_mutex to ensure atomic console output
            auto now = chrono::system_clock::now(); // Get current time
            auto now_c = chrono::system_clock::to_time_t(now); // Convert to time_t for formatting
            auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000; // Extract milliseconds
            // Print "thinking" state with timestamp and indentation (2 spaces per philosopher ID)
            cout << put_time(localtime(&now_c), "%H:%M:%S") << "." << setfill('0') << setw(3) << ms.count()
                 << " " << string(id * 2, ' ') << "P" << id << ": T" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000)); // Simulate thinking for 1 second

        // Hungry state
        {
            lock_guard<mutex> lock(cout_mutex); // Lock cout_mutex for safe output
            auto now = chrono::system_clock::now();
            auto now_c = chrono::system_clock::to_time_t(now);
            auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
            // Print "hungry" state, indicating the philosopher wants to eat
            cout << put_time(localtime(&now_c), "%H:%M:%S") << "." << setfill('0') << setw(3) << ms.count()
                 << " " << string(id * 2, ' ') << "P" << id << ": H" << endl;
        }

        // Attempt to pick up forks (critical section)
        if (left_fork < right_fork) { // Lock forks in ascending order to prevent deadlock
            forks[left_fork]->lock();   // Lock the left fork first
            forks[right_fork]->lock();  // Then lock the right fork
        } else {                        // For the last philosopher (e.g., P4), reverse order
            forks[right_fork]->lock();  // Lock the right fork first (forks[0])
            forks[left_fork]->lock();   // Then lock the left fork (forks[4])
        }

        // Eating state
        {
            lock_guard<mutex> lock(cout_mutex); // Lock cout_mutex for safe output
            auto now = chrono::system_clock::now();
            auto now_c = chrono::system_clock::to_time_t(now);
            auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
            // Print "eating" state with fork indices to show which forks are used
            cout << put_time(localtime(&now_c), "%H:%M:%S") << "." << setfill('0') << setw(3) << ms.count()
                 << " " << string(id * 2, ' ') << "P" << id << ": E (forks " << left_fork << " and " << right_fork << ")" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000)); // Simulate eating for 1 second

        // Releasing forks
        {
            lock_guard<mutex> lock(cout_mutex); // Lock cout_mutex for safe output
            auto now = chrono::system_clock::now();
            auto now_c = chrono::system_clock::to_time_t(now);
            auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
            // Print message indicating forks are being released
            cout << put_time(localtime(&now_c), "%H:%M:%S") << "." << setfill('0') << setw(3) << ms.count()
                 << " " << string(id * 2, ' ') << "P" << id << ": unlocking forks " << left_fork << " and " << right_fork << endl;
        }
        forks[left_fork]->unlock();   // Release the left fork
        forks[right_fork]->unlock();  // Release the right fork
    }
}

// Main function to set up and run the simulation
int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <number_of_philosophers>" << endl;
        return 1; // Exit with error code if usage is incorrect
    }

    // Convert argument to integer and validate
    int num_philosophers = atoi(argv[1]);
    if (num_philosophers < 2) {
        cout << "Number of philosophers must be at least 2" << endl;
        return 1; // Exit if too few philosophers are specified
    }

    // Initialize the forks vector with unique mutexes
    for (int i = 0; i < num_philosophers; i++) {
        forks.emplace_back(make_unique<mutex>()); // Create a new mutex for each fork
    }

    // Create a vector to hold philosopher threads
    vector<thread> philosophers;
    for (int i = 0; i < num_philosophers; i++) {
        philosophers.emplace_back(philosopher, i); // Start a thread for each philosopher with their ID
    }

    // Run the simulation for 10 seconds
    this_thread::sleep_for(chrono::seconds(10));

    // Detach threads to allow them to run independently (simulation ends after 10s anyway)
    for (auto& t : philosophers) {
        t.detach(); // Detach threads instead of joining to let them run freely
    }

    // Print end of simulation message
    cout << "Simulation ended" << endl;
    return 0; // Exit successfully
}