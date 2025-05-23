#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
using namespace std;

// ========== Component ==========
class PlaneComponent {
public:
    virtual int getBaggageWeight() const = 0;
    virtual void printInfo() const = 0;
    virtual void removeExcessBaggage(int& excessWeight) {}  // только у эконом-пассажиров
    virtual ~PlaneComponent() = default;
};

// ========== Leaf элементы ==========
class Pilot : public PlaneComponent {
public:
    int getBaggageWeight() const override { return 0; }
    void printInfo() const override {
        cout << "[Pilot] No baggage\n";
    }
};

class Stewardess : public PlaneComponent {
public:
    int getBaggageWeight() const override { return 0; }
    void printInfo() const override {
        cout << "[Stewardess] No baggage\n";
    }
};

class Passenger : public PlaneComponent {
protected:
    string name;
    int baggage;
public:
    Passenger(string n, int b) : name(n), baggage(b) {}
    int getBaggageWeight() const override { return baggage; }
    virtual void printInfo() const override = 0;
    virtual void removeExcessBaggage(int& excessWeight) override {}
};

class FirstClassPassenger : public Passenger {
public:
    FirstClassPassenger(string n, int b) : Passenger(n, b) {}
    void printInfo() const override {
        cout << "[1st Class] " << name << " | Baggage: " << baggage << " kg (allowed: unlimited)\n";
    }
};

class BusinessClassPassenger : public Passenger {
public:
    BusinessClassPassenger(string n, int b) : Passenger(n, b) {}
    void printInfo() const override {
        cout << "[Business] " << name << " | Baggage: " << baggage << " kg (allowed: 35)\n";
    }
};

class EconomyClassPassenger : public Passenger {
public:
    EconomyClassPassenger(string n, int b) : Passenger(n, b) {}
    void printInfo() const override {
        cout << "[Economy] " << name << " | Baggage: " << baggage << " kg (allowed: 20)\n";
    }

    void removeExcessBaggage(int& excessWeight) override {
        if (excessWeight <= 0 || baggage <= 20) return;
        int toRemove = min(excessWeight, baggage - 20);
        baggage -= toRemove;
        excessWeight -= toRemove;
        cout << "  [!] Removed " << toRemove << " kg from " << name << "'s baggage.\n";
    }
};

// ========== Composite ==========
class PassengerGroup : public PlaneComponent {
protected:
    vector<shared_ptr<PlaneComponent>> members;
public:
    void add(shared_ptr<PlaneComponent> c) {
        members.push_back(c);
    }

    int getBaggageWeight() const override {
        int total = 0;
        for (const auto& m : members)
            total += m->getBaggageWeight();
        return total;
    }

    void printInfo() const override {
        for (const auto& m : members)
            m->printInfo();
    }

    void removeExcessBaggage(int& excessWeight) override {
        for (auto& m : members) {
            m->removeExcessBaggage(excessWeight);
            if (excessWeight <= 0) break;
        }
    }
};

class Plane : public PassengerGroup {
    int maxBaggageWeight;
public:
    Plane(int maxWeight) : maxBaggageWeight(maxWeight) {}

    void loadSummary() {
        cout << "\n======= Plane Load Summary =======\n";
        printInfo();
        int total = getBaggageWeight();
        cout << "----------------------------------\n";
        cout << "Total baggage: " << total << " kg\n";
        cout << "Allowed: " << maxBaggageWeight << " kg\n";
        if (total > maxBaggageWeight) {
            int excess = total - maxBaggageWeight;
            cout << ">>> EXCESS: " << excess << " kg\n";
            cout << "Attempting to remove baggage from economy passengers...\n";
            removeExcessBaggage(excess);
            total = getBaggageWeight();
            cout << "Recalculated baggage: " << total << " kg\n";

if (total > maxBaggageWeight)
                cout << ">>> STILL OVERWEIGHT!\n";
            else
                cout << ">>> OK. Plane ready to fly.\n";
        } else {
            cout << ">>> OK. Plane ready to fly.\n";
        }
        cout << "==================================\n";
    }
};

int main() {
    auto plane = make_shared<Plane>(1000);  // Макс. вес багажа: 1000 кг

    // Экипаж
    plane->add(make_shared<Pilot>());
    plane->add(make_shared<Pilot>());
    for (int i = 0; i < 6; ++i)
        plane->add(make_shared<Stewardess>());

    // Первый класс
    auto firstClass = make_shared<PassengerGroup>();
    for (int i = 0; i < 5; ++i)
        firstClass->add(make_shared<FirstClassPassenger>("First_" + to_string(i+1), 40 + i * 2));
    plane->add(firstClass);

    // Бизнес класс
    auto businessClass = make_shared<PassengerGroup>();
    for (int i = 0; i < 10; ++i)
        businessClass->add(make_shared<BusinessClassPassenger>("Business_" + to_string(i+1), 30 + (i % 3) * 5));
    plane->add(businessClass);

    // Эконом класс (превышаем лимит багажа намеренно)
    auto economyClass = make_shared<PassengerGroup>();
    for (int i = 0; i < 20; ++i)
        economyClass->add(make_shared<EconomyClassPassenger>("Eco_" + to_string(i+1), 25 + (i % 4) * 5));
    plane->add(economyClass);

    // Запуск
    plane->loadSummary();

    return 0;
}