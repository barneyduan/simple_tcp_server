#include <iostream>
#include <string>
#include <mutex>
#include <random>
#include <chrono>
#include <thread>
#include "utils.h"

std::mutex local_mutex;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 2);

int calculation1(int level) {
    if (level > 10) {
        return level;
    }
    return calculation1(level + 1) + level;
}

int calculation2() {
    int sum = 0;
    for (size_t i = 0; i >= 0; ++i) {
        sum += std::pow(i, 2);
    }
    return sum;
}

std::string randomized_calculation() {
    int rand_num = dis(gen);
    switch (rand_num) {
        case 0:
        case 1:
            return std::to_string(calculation2());
        default:
            return "Static content with random number: " + std::to_string(rand_num) + " and this repesponse is for invalid request.";;
    }
}

std::string generate_response() {
    std::lock_guard<std::mutex> lock(local_mutex);

    std::string response = "Response generated with ID: ";
    response += randomized_calculation();
    response += "\n";

    return response;
}
