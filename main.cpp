#include <iostream>
#include <ctime>
#include <thread>
#include <vector>
#include <mutex>

std::mutex access1;
std::mutex access2;

class Delivery {
  private:
    std::vector<std::string> finishedDish;
    int index = 0;
  public:
    Delivery() {
        for (int i = 0;i < 4;i++) {
            finishedDish.emplace_back("unknown");
        }
    }

    void setDish (std::string &dish) {
        std::cout << "Position " << index << ": delivery get finished dish - " << dish << std::endl;
        finishedDish[index] = dish;
        index++;
    }
};

class Courier {
public:
    void sendingOrder (std::vector<std::string> &dishList) {
        for (int i = 0;i < 4;) {
            if (dishList[i] != "unknown") {
                std::this_thread::sleep_for(std::chrono::seconds(30));
                std::cout << "Position " << i << ": courier get order. Sending to the customer dish - " << dishList[i] << std::endl;
                i++;
            }
        }
    }
};

class Kitchen {
private:
    Delivery* delivery = new Delivery;
    bool status = false;
    std::string dish;
public:
    bool getStatus () {
        return status;
    }

    void setDish (std::string inDish, int ind) {
        dish = inDish;
        show(ind);
        access2.lock();
        status = true;
        access2.unlock();
        cooking(ind);
    }

    void show (int i) {
        std::cout << "Position " << i << ": kitchen get dish - " << dish << std::endl;
    }

    void cooking (int i) {
        std::cout << "Position " << i << ": kitchen cooking dish - " << dish << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(std::rand() % 16 + 5));
        std::cout << "Position " << i << ": kitchen cooked dish - " << dish << std::endl;
        std::cout << "Position " << i << ": kitchen sent to delivery dish - " << dish << std::endl;
        access2.lock();
        status = false;
        access2.unlock();
        delivery->setDish(dish);
    }

    void clear () {
        delete delivery;
    }
};

class Waiter {
private:
    Kitchen* kitchen = new Kitchen;
    int index = 0;
public:
    void setDish (int inDish, std::vector<std::string> &dishList) {
        access1.lock();
        if (inDish == 0) {
            dishList[index]  = "pizza";
            show (dishList);
        } else if (inDish == 1) {
            dishList[index]  = "soup";
            show (dishList);
        } else if (inDish == 2) {
            dishList[index]  = "steak";
            show (dishList);
        } else if (inDish == 3) {
            dishList[index]  = "salad";
            show (dishList);
        } else {
            dishList[index]  = "sushi";
            show (dishList);
        }
        access1.unlock();
    }

    void show (std::vector<std::string> &dishList) {
        std::cout << "Position " << index << ": the waiter received the order - " << dishList[index] << std::endl;
        index++;
    }

    void sendingOrder (std::vector<std::string> &dishList) {
        for (int i = 0;i < 4;) {
            if (dishList[i] != "unknown" && !kitchen->getStatus()) {
                std::cout << "Position " << i << ": waiter sent to kitchen dish - " << dishList[i] << std::endl;
                kitchen->setDish(dishList[i], i);
                i++;
            }
        }
    }

    void clear () {
        kitchen->clear();
        delete kitchen;
    }


};

void orderAcceptance (Waiter* waiter, std::vector<std::string> &dishList) {
    for (int i = 0;i < 4;i++) {
        std::srand(std::time(nullptr));
        int t = std::rand() % 10 + 5;
        std::cout << "Position " << i << ": order waiting time - " << t << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(t));
        int num = std::rand() % 5;
        std::cout << "Position " << i << ": order number - " << num << std::endl;
        waiter->setDish(num, dishList);
    }
}

void dishListInit (std::vector<std::string> &dishList) {
    for (int i = 0;i < 4;i++) {
        dishList.emplace_back("unknown");
    }
}

int main() {
    std::vector<std::string> dishList;
    dishListInit (dishList);
    Waiter* waiter = new Waiter;
    Courier* courier = new Courier;
    std::thread th1(orderAcceptance, std::ref(waiter), std::ref(dishList));
    std::thread th2(&Waiter::sendingOrder, waiter, std::ref(dishList));
    std::thread th3(&Courier::sendingOrder, courier, std::ref(dishList));
    waiter->clear();
    delete waiter;
    delete courier;
    th1.join();
    th2.join();
    th3.join();
    return 0;
}