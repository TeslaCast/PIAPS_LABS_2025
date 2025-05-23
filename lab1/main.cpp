
#include <iostream>
#include <vector>
#include <memory>

using namespace std;

// ===================== Пассажиры =====================

class Passenger {
public:
    virtual string type() const = 0;
    virtual ~Passenger() = default;
};

class BusPassenger : public Passenger {
public:
    string type() const override {
        return "Bus passenger";
    }
};

class TaxiPassenger : public Passenger {
public:
    string type() const override {
        return "Taxi passenger";
    }
};

// ===================== Водители (Singleton) =====================

class Driver {
protected:
    Driver() = default;
public:
    virtual string category() const = 0;
    virtual ~Driver() = default;
};

class BusDriver : public Driver {
private:
    static BusDriver* instance;
    BusDriver() {}
public:
    static BusDriver* getInstance() {
        if (!instance)
            instance = new BusDriver();
        return instance;
    }
    string category() const override {
        return "Bus Driver (Category D)";
    }
};

class TaxiDriver : public Driver {
private:
    static TaxiDriver* instance;
    TaxiDriver() {}
public:
    static TaxiDriver* getInstance() {
        if (!instance)
            instance = new TaxiDriver();
        return instance;
    }
    string category() const override {
        return "Taxi Driver (Category B)";
    }
};

BusDriver* BusDriver::instance = nullptr;
TaxiDriver* TaxiDriver::instance = nullptr;

// ===================== Транспорт =====================

class Vehicle {
public:
    virtual void loadPassenger(shared_ptr<Passenger> p) = 0;
    virtual void assignDriver(Driver* d) = 0;
    virtual void status() const = 0;
    virtual ~Vehicle() = default;
};

class Bus : public Vehicle {
    vector<shared_ptr<Passenger>> passengers;
    Driver* driver = nullptr;
    static const int capacity = 30;
public:
    void loadPassenger(shared_ptr<Passenger> p) override {
        if (passengers.size() < capacity)
            passengers.push_back(p);
    }

    void assignDriver(Driver* d) override {
        if (!driver)
            driver = d;
    }

    void status() const override {
        cout << "--- Bus Status ---\n";
        cout << "Driver: " << (driver ? driver->category() : "None") << "\n";
        cout << "Passengers: " << passengers.size() << "/" << capacity << "\n";
    }
};

class Taxi : public Vehicle {
    vector<shared_ptr<Passenger>> passengers;
    Driver* driver = nullptr;
    static const int capacity = 4;
public:
    void loadPassenger(shared_ptr<Passenger> p) override {
        if (passengers.size() < capacity)
            passengers.push_back(p);
    }

    void assignDriver(Driver* d) override {
        if (!driver)
            driver = d;
    }

    void status() const override {
        cout << "--- Taxi Status ---\n";
        cout << "Driver: " << (driver ? driver->category() : "None") << "\n";
        cout << "Passengers: " << passengers.size() << "/" << capacity << "\n";
    }
};

// ===================== Abstract Factory =====================

class TransportFactory {
public:
    virtual Vehicle* createVehicle() = 0;
    virtual Passenger* createPassenger() = 0;
    virtual Driver* getDriver() = 0;
    virtual ~TransportFactory() = default;
};

class BusFactory : public TransportFactory {
public:
    Vehicle* createVehicle() override {
        return new Bus();
    }

    Passenger* createPassenger() override {
        return new BusPassenger();
    }

    Driver* getDriver() override {
        return BusDriver::getInstance();
    }
};

class TaxiFactory : public TransportFactory {
public:
    Vehicle* createVehicle() override {
        return new Taxi();
    }

    Passenger* createPassenger() override {
        return new TaxiPassenger();
    }

    Driver* getDriver() override {
        return TaxiDriver::getInstance();
    }
};

// ===================== Главная программа =====================

void simulate(TransportFactory* factory, int passengerCount) {
    Vehicle* vehicle = factory->createVehicle();
    vehicle->assignDriver(factory->getDriver());
    for (int i = 0; i < passengerCount; ++i) {
        vehicle->loadPassenger(shared_ptr<Passenger>(factory->createPassenger()));
    }

    vehicle->status();
    delete vehicle;
}

int main() {
    BusFactory busFactory;
    TaxiFactory taxiFactory;

    simulate(&busFactory, 28);
    cout << endl;
    simulate(&taxiFactory, 3);

    return 0;
}
