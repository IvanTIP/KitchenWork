#include <iostream>
#include <ctime>
#include <thread>
#include <vector>
#include <mutex>

std::mutex access1;
std::mutex access2;
std::mutex access3;

class Courier {
private:
    //Delivery* delivery = new Delivery;
public:
    void sendingOrder (std::string &dish, int inI) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        std::cout << inI << " Courier get " << dish << std::endl;
    }
};

class Delivery {
  private:
    //Courier* courier = new Courier;
    std::vector<std::string> finishedDish;
    int index = 0;
    bool status = false;
  public:
    Delivery () {
        for (int i = 0;i < 4;i++) {
            finishedDish[i] = "unknown";
        }
    }


    void getToCourier () {
        for (int i = 0;i < 4;) {
            if (finishedDish[index] != "unknown") {
                courier->sendingOrder(finishedDish[index], i);
                i++;
            }
        }
    }

    void setDish (std::string &dish) {
        std::cout << index << " Delivery get " << dish << std::endl;
        finishedDish[index] = dish;
        index++;
        //finishedDish.push_back(dish);
        access1.lock();
        status = true;
        access1.unlock();
    }

    bool getStatus () {
        return status;
    }

    void setStatus (bool inStatus) {
        status = inStatus;
    }

     std::string getDish (int inI) {
        return finishedDish[inI];
    }
};



class Kitchen {
private:
    Delivery* delivery = new Delivery;
    bool status = false;
    std::string dish;
    int index = 0;
public:
    bool getStatus () {
        return status;
    }

    void setDish (std::string inDish, int ind) {
        dish = inDish;
        show(ind);
        access3.lock();
        status = true;
        access3.unlock();
        cooking(ind);

    }

    void show (int i) {
        std::cout << i << " Kitchen get " << dish << std::endl;
    }

    void cooking (int i) {

        std::cout << i << " Kitchen cooking " << dish << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(std::rand() % 16 + 5));
        std::cout << i << " Kitchen cooked " << dish << std::endl;


        std::cout << i << " Kitchen sent to delivery " << dish << std::endl;
        access3.lock();
        status = false;
        delivery->setDish(dish);
        access3.unlock();
    }
};

class Waiter {
private:
    Kitchen* kitchen = new Kitchen;
    std::vector<std::string> dish;
    bool dishAdd = false;
    int index = 0;
public:
    void setDish (int inDish) {
        access2.lock();
        if (inDish == 0) {
            dish.emplace_back("pizza");
            dishAdd = true;
            show ();
        } else if (inDish == 1) {
            dish.emplace_back("soup");
            dishAdd = true;
            show ();
        } else if (inDish == 2) {
            dish.emplace_back("steak");
            dishAdd = true;
            show ();
        } else if (inDish == 3) {
            dish.emplace_back("salad");
            dishAdd = true;
            show ();
        } else {
            dish.emplace_back("sushi");
            dishAdd = true;
            show ();
        }
        access2.unlock();
    }

    void show () {
        std::cout << index << " The waiter received the order: " << dish[dish.size() - 1] << std::endl;
    }

    void sendingOrder () {
        while (index < 4) {
            if (dishAdd && !kitchen->getStatus()) {
                std::cout << index << " Waiter sent  to kitchen " << dish[index] << std::endl;
                kitchen->setDish(dish[index], index);
                index++;
                dishAdd = false;
            }
        }
    }

};

void orderAcceptance (Waiter* waiter) {
    for (int i = 0;i < 4;i++) {
        std::srand(std::time(nullptr));
        int t = std::rand() % 10 + 5;
        std::cout << i << " Time " << t << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(t));
        int d = std::rand() % 5;
        std::cout << i << " Dish num " << d << std::endl;
        waiter->setDish(d);
    }
}

int main() {
    Waiter* waiter = new Waiter;
    //Courier* courier = new Courier;
    std::thread th1(orderAcceptance, std::ref(waiter));
    std::thread th2(&Waiter::sendingOrder, waiter);
    //std::thread th3(&Kitchen::sentDish, kitchen);
    std::thread th3(&Delivery::getToCourier, delivery);
    th1.join();
    th2.join();
    th3.join();
    return 0;
}