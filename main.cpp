#include <iostream>
#include <ctime>
#include <thread>
#include <vector>
#include <mutex>

std::mutex access1;
std::mutex access2;

void orderReceipt (std::vector<std::string> &order) {
    for (int i = 0;i < 4;i++) {
        std::srand(std::time(nullptr));
        std::this_thread::sleep_for(std::chrono::seconds(std::rand() % 10 + 5));
        int dishNum = std::rand() % 5;
        access2.lock();
        if (dishNum == 0) {
            order[i] = "pizza";
            std::cout <<  "The waiter accepted the order: pizza" << std::endl;
        } else if (dishNum == 1) {
            order[i] = "soup";
            std::cout << "The waiter accepted the order: soup" << std::endl;
        } else if (dishNum == 2) {
            order[i] = "steak";
            std::cout << "The waiter accepted the order: steak" << std::endl;
        } else if (dishNum == 3) {
            order[i] = "salad";
            std::cout << "The waiter accepted the order: salad" << std::endl;
        } else {
            order[i] = "sushi";
            std::cout << "The waiter accepted the order: sushi" << std::endl;
        }
        access2.unlock();
    }
}

void kitchen (std::vector<std::string> &order, std::vector<std::string> &finishedDish) {
    for (int i = 0;i < 4;) {
        access2.lock();
        if (order[i] != "unknown") {
            std::cout << "The kitchen accepted the order: " << order[i] << std::endl;
            std::cout << "Cooking " << order[i] << std::endl;
            access2.unlock();
            std::srand(std::time(nullptr));
            std::this_thread::sleep_for(std::chrono::seconds(std::rand() % 15 + 5));
            access2.lock();
            std::cout << "The " << order[i] << " is ready" << std::endl;
            finishedDish[i] = order[i];
            access2.unlock();
            i++;
        } else {
            access2.unlock();
        }
    }
}

void courier (std::vector<std::string> &finishedDish) {
    for (int i = 0;i < 4;) {
        access2.lock();
        if (finishedDish[i] != "unknown") {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            std::cout << "The courier accepted the order: " << finishedDish[i] << std::endl;
            i++;
        }
        access2.unlock();
    }
}

void dishListsInit (std::vector<std::string> &order, std::vector<std::string> &finishedDish) {
    for (int i = 0;i < 4;i++) {
        order.emplace_back("unknown");
        finishedDish.emplace_back("unknown");
    }
}

int main() {
    std::vector<std::string> order;
    std::vector<std::string> finishedDish;
    access1.lock();
    dishListsInit(order, finishedDish);
    access1.unlock();
    access1.lock();
    std::thread th1(orderReceipt, std::ref(order));
    std::thread th2(kitchen, std::ref(order), std::ref(finishedDish));
    std::thread th3(courier, std::ref(finishedDish));
    th1.join();
    th2.join();
    th3.join();
    access1.unlock();
    return 0;
}