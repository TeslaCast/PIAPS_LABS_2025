#include <iostream>
#include <vector>
#include <memory>

using namespace std;

// ========== Пассажиры ==========
class Passenger {
public:
    virtual string type() const = 0;
    virtual double ticketPrice() const = 0;
    virtual ~Passenger() = default;
};

class AdultBusPassenger : public Passenger {
public:
    string type() const override { return "Adult (Bus)"; }
    double ticketPrice() const override { return 100.0; }
};

class ChildBusPassenger : public Passenger {
public:
    string type() const override { return "Child (Bus)"; }
    double ticketPrice() const override { return 50.0; }
};

class DiscountedBusPassenger : public Passenger {
public:
    string type() const override { return "Discounted (Bus)"; }
    double ticketPrice() const override { return 30.0; }
};

class AdultTaxiPassenger : public Passenger {
public:
    string type() const override { return "Adult (Taxi)"; }
    double ticketPrice() const override { return 200.0; }
};

class ChildTaxiPassenger : public Passenger {
public:
    string type() const override { return "Child (Taxi) — requires seat"; }
    double ticketPrice() const override { return 200.0; }
};

// ========== Водители (Singleton) ==========
class Driver {
protected:
    Driver() {}
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
    string category() const override { return "Bus Driver (Category D)"; }
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
    string category() const override { return "Taxi Driver (Category B)"; }
};

BusDriver* BusDriver::instance = nullptr;
TaxiDriver* TaxiDriver::instance = nullptr;

// ========== Транспорт ==========
class Vehicle {
protected:
    Driver* driver = nullptr;
    vector<shared_ptr<Passenger>> passengers;
public:
    virtual void setDriver(Driver* d) { driver = d; }
    virtual void addPassenger(shared_ptr<Passenger> p) {
        passengers.push_back(p);
    }
    virtual void status() const = 0;
    virtual ~Vehicle() = default;
};

class Bus : public Vehicle {
    static const int capacity = 30;
public:
    void addPassenger(shared_ptr<Passenger> p) override {
        if (passengers.size() < capacity)
            passengers.push_back(p);
    }

    void status() const override {
        cout << "\n--- Bus Status ---\n";
        cout << "Driver: " << (driver ? driver->category() : "None") << "\n";
        cout << "Passengers: " << passengers.size() << "/" << capacity << "\n";
        double total = 0;
        for (const auto& p : passengers) {
            cout << "- " << p->type() << ", ticket: " << p->ticketPrice() << "\n";
            total += p->ticketPrice();
        }
        cout << "Total income: " << total << "\n";
    }
};

class Taxi : public Vehicle {
    static const int capacity = 4;
public:
    void addPassenger(shared_ptr<Passenger> p) override {
        if (passengers.size() < capacity)
            passengers.push_back(p);
    }

    void status() const override {
        cout << "\n--- Taxi Status ---\n";
        cout << "Driver: " << (driver ? driver->category() : "None") << "\n";
        cout << "Passengers: " << passengers.size() << "/" << capacity << "\n";
        for (const auto& p : passengers)
            cout << "- " << p->type() << "\n";
    }
};

// ========== Builder ==========
class TransportBuilder {
public:
    virtual void createTransport() = 0;
    virtual void addDriver() = 0;
    virtual void addPassengers() = 0;
    virtual Vehicle* getResult() = 0;
    virtual ~TransportBuilder() = default;
};

class BusBuilder : public TransportBuilder {
    Bus* bus;
public:
    void createTransport() override { bus = new Bus(); }
    void addDriver() override { bus->setDriver(BusDriver::getInstance()); }
    void addPassengers() override {
        bus->addPassenger(make_shared<AdultBusPassenger>());
        bus->addPassenger(make_shared<ChildBusPassenger>());
        bus->addPassenger(make_shared<DiscountedBusPassenger>());
    }
    Vehicle* getResult() override { return bus; }
};

class TaxiBuilder : public TransportBuilder {
    Taxi* taxi;
public:
    void createTransport() override { taxi = new Taxi(); }
    void addDriver() override { taxi->setDriver(TaxiDriver::getInstance()); }
    void addPassengers() override {
        taxi->addPassenger(make_shared<AdultTaxiPassenger>());
        taxi->addPassenger(make_shared<ChildTaxiPassenger>());
    }
    Vehicle* getResult() override { return taxi; }
};

// ========== Director ==========
class TransportDirector {
public:
    Vehicle* buildTransport(TransportBuilder& builder) {
        builder.createTransport();
        builder.addDriver();
        builder.addPassengers();
        return builder.getResult();
    }
};

// ========== Main ==========
int main() {
    TransportDirector director;

    BusBuilder busBuilder;
    TaxiBuilder taxiBuilder;

    Vehicle* bus = director.buildTransport(busBuilder);
    Vehicle* taxi = director.buildTransport(taxiBuilder);

    bus->status();
    taxi->status();

    delete bus;
    delete taxi;
    return 0;
}