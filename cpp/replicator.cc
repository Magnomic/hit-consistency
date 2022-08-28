#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <functional>

#include "replicator.h"


Replicator::Replicator(){
};

Replicator::~Replicator(){};

Replicator& Replicator::getInstance(){
    static Replicator instance;
        return instance;
}

void Replicator::run(std::function<int(void)> f, int duration){
    std::thread([f, duration]() {
        while (true)
        {
            f();
            auto ms = std::chrono::steady_clock::now() + std::chrono::milliseconds(duration);
            std::this_thread::sleep_until(ms);
        }
    }).detach();
}

int Replicator::_send_empty_entries(){
    
    RequestVoteRequest request;
    RequestVoteResponse response;

    return 0;

}