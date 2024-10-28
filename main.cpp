#include "Graph.hpp"
#include "MSTStrategy.hpp"
#include "MSTree.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include "pollserver.hpp"
using namespace std;

#define PORT "9034"
void runPollServer(const char* port, std::atomic<bool>& exit_flag) {
    poll_clients(port, exit_flag);
}

int main()
{
    // Create an atomic flag for signaling exit
    atomic<bool> exit_flag(false);

    // Start poll_clients in a new thread, passing the exit flag
    thread pollThread([&]() { runPollServer(PORT, exit_flag); });

    // Wait for user to press Enter
    cout << "Press Enter to exit..." << endl;
    cin.get();
    // Set the flag to true, signaling poll_clients to exit
    exit_flag.store(true);

    // Ensure the thread is joinable and then join it
    if (pollThread.joinable()) {
        pollThread.join();
    }
    return 0;
}
