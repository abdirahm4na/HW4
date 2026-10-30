/*
 * Full Name:     Abdirahman Abanoor
 * Student ID:    002046243
 * Course:        EECE 2140 - Computing Fundamentals for Engineers
 * Section:       05
 * Semester:      Fall 2026
 * Assignment:    Homework 4 - Parking Garage
 * Compilation:   g++ -std=c++11 main.cpp -o main
 * Description:   A parking-garage program. It defines a ClockTime
 *                structure, a Vehicle class that records one stay in the
 *                garage, and three classes derived from Vehicle (Car,
 *                Motorcycle, and Bicycle) that each compute their own
 *                parking fee in cents.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

// ---------------------------------------------------------------------
// Global named constants (provided in the template)
// ---------------------------------------------------------------------
const int MINUTES_PER_HOUR = 60;
const int HOURS_PER_DAY = 24;
const int MAX_TICKET = 9999;              // largest valid ticket number
const int CAR_SPACES = 3;                 // spaces in the garage, by type
const int MOTORCYCLE_SPACES = 2;
const int BICYCLE_RACKS = 4;              // racks are numbered 1 to 4
const int MAX_RECORDS = 20;               // vehicles of each type per day
const int CAR_RATE_CENTS = 300;           // per billed hour
const int CAR_DAILY_MAX_CENTS = 1800;     // most a car pays per stay
const int CHARGING_FEE_CENTS = 400;       // electric cars, once per stay
const int MOTORCYCLE_RATE_CENTS = 150;    // per billed hour
const int MOTORCYCLE_DAILY_MAX_CENTS = 900;
const int BICYCLE_FREE_HOURS = 2;         // billed hours a bicycle parks free
const int BICYCLE_RATE_CENTS = 50;        // per billed hour after that
const std::string LOG_FILE_NAME = "parking_log.txt";

// ---------------------------------------------------------------------
// Structure and function declarations (provided in the template)
// ---------------------------------------------------------------------
struct ClockTime
{
    int hour;     // 0 to 23 (24-hour clock)
    int minute;   // 0 to 59
};

ClockTime makeTime(int hour, int minute);
bool isValidTime(ClockTime t);
int minutesBetween(ClockTime from, ClockTime to);
int billedHours(int minutes);

// ---------------------------------------------------------------------
// Class Vehicle: one vehicle's stay in the garage on one day. A vehicle
// is parked from the time it is constructed until a successful
// checkOut. Times are on a 24-hour clock, and a stay never crosses
// midnight.
// ---------------------------------------------------------------------
class Vehicle
{
public:
    Vehicle();
    // A placeholder vehicle: ticket 0, entered at 0:00, still parked.

    Vehicle(int ticket, ClockTime entry);
    // A vehicle with ticket number ticket that entered at entry and is
    // still parked. If ticket is not between 0 and MAX_TICKET, or entry
    // is not a valid time, prints "Invalid vehicle setup." and ends the
    // program with exit status 1.

    int getTicket();
    // Returns the ticket number.

    bool isParked();
    // Returns true until the vehicle has been checked out successfully.

    int checkOut(ClockTime exitTime);
    // If the vehicle is parked, exitTime is a valid time, and exitTime is
    // not earlier than the entry time, records the check-out and returns
    // the number of minutes parked. Otherwise changes nothing and
    // returns -1.

    int checkOut(int hour, int minute);
    // Same as checkOut(makeTime(hour, minute)).

    int getMinutesParked();
    // Returns the minutes from entry to check-out after a successful
    // checkOut, or 0 while the vehicle is still parked.

    void output(std::ostream& outs);
    // Writes one line to outs, for example:
    // Ticket 101: in 8:05, parked
    // Ticket 101: in 8:05, out 10:40 (155 min)

private:

    // TODO A: Declare the private member variables that store a
    //         Vehicle's state, and declare any private helper member
    //         functions you add.
    int ticket_;                 // Ticket and entry/exit times for this stay.
    ClockTime entryTime_;
    ClockTime exitTime_;
    bool parked_;                // Changes only after a successful check-out.
    int minutesParked_;          // Recorded duration; zero while still parked.
};

// ---------------------------------------------------------------------
// Class Car: a Vehicle that may be electric (and pay for charging).
// ---------------------------------------------------------------------
class Car : public Vehicle
{
public:
    Car();
    // A placeholder car: as Vehicle(), not electric.

    Car(int ticket, ClockTime entry);
    // A car that is not electric; otherwise as Vehicle(ticket, entry).

    Car(int ticket, ClockTime entry, bool electric);
    // As Vehicle(ticket, entry); the car is electric if electric is true.

    bool isElectric();
    // Returns true for an electric car.

    int getFeeCents();
    // Returns 0 while the car is parked. After check-out: CAR_RATE_CENTS
    // for each billed hour, but at most CAR_DAILY_MAX_CENTS, plus
    // CHARGING_FEE_CENTS if the car is electric.

private:
    // TODO B: Declare the private member variables that Car adds to
    //         Vehicle, and any private helper member functions you add.
    bool electric_;
};

// ---------------------------------------------------------------------
// Class Motorcycle: a Vehicle with its own rates.
// ---------------------------------------------------------------------
class Motorcycle : public Vehicle
{
public:
    Motorcycle();
    // A placeholder motorcycle: as Vehicle().

    Motorcycle(int ticket, ClockTime entry);
    // As Vehicle(ticket, entry).

    int getFeeCents();
    // Returns 0 while the motorcycle is parked. After check-out:
    // MOTORCYCLE_RATE_CENTS for each billed hour, but at most
    // MOTORCYCLE_DAILY_MAX_CENTS.

private:
    // TODO C: Declare any private members that Motorcycle adds to
    //         Vehicle (it may need none).

};

// ---------------------------------------------------------------------
// Class Bicycle: a Vehicle locked to a numbered rack.
// ---------------------------------------------------------------------
class Bicycle : public Vehicle
{
public:
    Bicycle();
    // A placeholder bicycle: as Vehicle(), rack 0.

    Bicycle(int ticket, ClockTime entry, int rack);
    // As Vehicle(ticket, entry), locked to rack. If rack is not between
    // 1 and BICYCLE_RACKS, prints "Invalid vehicle setup." and ends the
    // program with exit status 1.

    int getRack();
    // Returns the rack number (0 for a placeholder).

    int getFeeCents();
    // Returns 0 while the bicycle is parked. After check-out: free for
    // the first BICYCLE_FREE_HOURS billed hours, then BICYCLE_RATE_CENTS
    // for each further billed hour.

private:
    // TODO D: Declare the private member variables that Bicycle adds to
    //         Vehicle, and any private helper member functions you add.
    int rack_;                   // Zero for a placeholder; otherwise 1 through BICYCLE_RACKS.
};

// ---------------------------------------------------------------------
// main() -- provided; do not modify anything above the line of "=" signs.
// ---------------------------------------------------------------------
int main()
{
    Car cars[MAX_RECORDS];                  // every car parked today
    Motorcycle motorcycles[MAX_RECORDS];    // every motorcycle parked today
    Bicycle bicycles[MAX_RECORDS];          // every bicycle parked today
    int carCount = 0, motorcycleCount = 0, bicycleCount = 0;
    int nextTicket = 101;
    int checkedOut = 0;
    int revenueCents = 0;
    char command = 'Q';

    std::cout.setf(std::ios::fixed);
    std::cout.setf(std::ios::showpoint);
    std::cout.precision(2);

    std::cout << "=== EECE 2140 Parking Garage ===" << std::endl;
    std::cout << "Spaces: " << CAR_SPACES << " cars, " << MOTORCYCLE_SPACES
              << " motorcycles, " << BICYCLE_RACKS << " bicycle racks" << std::endl;

    do
    {
        std::cout << std::endl << "Command (C, M, B, X, P, Q): ";
        if (!(std::cin >> command))
        {
            command = 'Q';   // end of input: close the garage
        }

        if (command == 'C' || command == 'M' || command == 'B' || command == 'X')
        {
            int ticket = 0, hour = -1, minute = -1, electric = 0;
            char colon = ' ';
            if (command == 'X')
            {
                std::cin >> ticket;
            }
            std::cin >> hour >> colon >> minute;
            if (command == 'C')
            {
                std::cin >> electric;
            }
            ClockTime when = makeTime(hour, minute);

            if (colon != ':' || !isValidTime(when))
            {
                std::cout << "Invalid time." << std::endl;
            }
            else if (command == 'C')
            {
                int parkedNow = 0;
                for (int i = 0; i < carCount; i++)
                {
                    if (cars[i].isParked())
                    {
                        parkedNow++;
                    }
                }
                if (parkedNow == CAR_SPACES || carCount == MAX_RECORDS)
                {
                    std::cout << "No car space available." << std::endl;
                }
                else
                {
                    cars[carCount] = Car(nextTicket, when, electric == 1);
                    carCount++;
                    std::cout << "Car parked. Ticket " << nextTicket << "." << std::endl;
                    nextTicket++;
                }
            }
            else if (command == 'M')
            {
                int parkedNow = 0;
                for (int i = 0; i < motorcycleCount; i++)
                {
                    if (motorcycles[i].isParked())
                    {
                        parkedNow++;
                    }
                }
                if (parkedNow == MOTORCYCLE_SPACES || motorcycleCount == MAX_RECORDS)
                {
                    std::cout << "No motorcycle space available." << std::endl;
                }
                else
                {
                    motorcycles[motorcycleCount] = Motorcycle(nextTicket, when);
                    motorcycleCount++;
                    std::cout << "Motorcycle parked. Ticket " << nextTicket << "." << std::endl;
                    nextTicket++;
                }
            }
            else if (command == 'B')
            {
                int rack = 0;   // lowest-numbered free rack, or 0 if none
                for (int r = 1; r <= BICYCLE_RACKS && rack == 0; r++)
                {
                    bool taken = false;
                    for (int i = 0; i < bicycleCount; i++)
                    {
                        if (bicycles[i].isParked() && bicycles[i].getRack() == r)
                        {
                            taken = true;
                        }
                    }
                    if (!taken)
                    {
                        rack = r;
                    }
                }
                if (rack == 0 || bicycleCount == MAX_RECORDS)
                {
                    std::cout << "No bicycle rack available." << std::endl;
                }
                else
                {
                    bicycles[bicycleCount] = Bicycle(nextTicket, when, rack);
                    bicycleCount++;
                    std::cout << "Bicycle parked at rack " << rack << ". Ticket "
                              << nextTicket << "." << std::endl;
                    nextTicket++;
                }
            }
            else   // command == 'X'
            {
                char type = ' ';
                int index = -1;
                for (int i = 0; i < carCount; i++)
                {
                    if (cars[i].isParked() && cars[i].getTicket() == ticket)
                    {
                        type = 'C';
                        index = i;
                    }
                }
                for (int i = 0; i < motorcycleCount; i++)
                {
                    if (motorcycles[i].isParked() && motorcycles[i].getTicket() == ticket)
                    {
                        type = 'M';
                        index = i;
                    }
                }
                for (int i = 0; i < bicycleCount; i++)
                {
                    if (bicycles[i].isParked() && bicycles[i].getTicket() == ticket)
                    {
                        type = 'B';
                        index = i;
                    }
                }

                int minutes = -1;
                if (type == 'C')
                {
                    minutes = cars[index].checkOut(hour, minute);
                }
                else if (type == 'M')
                {
                    minutes = motorcycles[index].checkOut(when);
                }
                else if (type == 'B')
                {
                    minutes = bicycles[index].checkOut(when);
                }

                if (index < 0)
                {
                    std::cout << "No parked vehicle has ticket " << ticket << "." << std::endl;
                }
                else if (minutes < 0)
                {
                    std::cout << "Check-out time is before the entry time." << std::endl;
                }
                else
                {
                    std::ofstream logStream;
                    logStream.open(LOG_FILE_NAME, std::ios::app);
                    if (logStream.fail())
                    {
                        std::cout << "Log file opening failed: " << LOG_FILE_NAME << std::endl;
                        exit(1);
                    }
                    logStream.setf(std::ios::fixed);
                    logStream.setf(std::ios::showpoint);
                    logStream.precision(2);

                    int fee = 0;
                    std::cout << "Checked out. ";
                    if (type == 'C')
                    {
                        fee = cars[index].getFeeCents();
                        cars[index].output(std::cout);
                        cars[index].output(logStream);
                    }
                    else if (type == 'M')
                    {
                        fee = motorcycles[index].getFeeCents();
                        motorcycles[index].output(std::cout);
                        motorcycles[index].output(logStream);
                    }
                    else
                    {
                        fee = bicycles[index].getFeeCents();
                        bicycles[index].output(std::cout);
                        bicycles[index].output(logStream);
                    }
                    std::cout << "Fee: $" << fee / 100.0 << std::endl;
                    logStream << "Fee: $" << fee / 100.0 << std::endl;
                    logStream.close();

                    checkedOut++;
                    revenueCents += fee;
                }
            }
        }
        else if (command == 'P')
        {
            std::cout << "Cars:" << std::endl;
            for (int i = 0; i < carCount; i++)
            {
                if (cars[i].isParked())
                {
                    std::cout << "  ";
                    cars[i].output(std::cout);
                }
            }
            std::cout << "Motorcycles:" << std::endl;
            for (int i = 0; i < motorcycleCount; i++)
            {
                if (motorcycles[i].isParked())
                {
                    std::cout << "  ";
                    motorcycles[i].output(std::cout);
                }
            }
            std::cout << "Bicycles:" << std::endl;
            for (int i = 0; i < bicycleCount; i++)
            {
                if (bicycles[i].isParked())
                {
                    std::cout << "  Rack " << bicycles[i].getRack() << ": ";
                    bicycles[i].output(std::cout);
                }
            }
        }
        else if (command != 'Q')
        {
            std::cout << "Unknown command: " << command << std::endl;
        }
    } while (command != 'Q');

    int stillParked = 0;
    for (int i = 0; i < carCount; i++)
    {
        if (cars[i].isParked())
        {
            stillParked++;
        }
    }
    for (int i = 0; i < motorcycleCount; i++)
    {
        if (motorcycles[i].isParked())
        {
            stillParked++;
        }
    }
    for (int i = 0; i < bicycleCount; i++)
    {
        if (bicycles[i].isParked())
        {
            stillParked++;
        }
    }

    std::cout << std::endl << "=== End of Day ===" << std::endl;
    std::cout << "Vehicles checked out: " << checkedOut << std::endl;
    std::cout << "Revenue: $" << revenueCents / 100.0 << std::endl;
    std::cout << "Vehicles still parked: " << stillParked << std::endl;

    return 0;
}

// =====================================================================
// Complete TODO A through TODO D in the class definitions above, then
// each definition below. Do not change the public section of any class,
// the ClockTime structure, any function's name, return type, or
// parameter list, main(), or the provided constants. Each placeholder
// "return" statement is there only so that the template compiles;
// replace it with your own code.
// =====================================================================

// ---- ClockTime functions --------------------------------------------

// TODO 1: makeTime
//   Parameters: hour, minute -- a time of day.
//   Returns:    A ClockTime whose hour member is hour and whose minute
//               member is minute. The values are not checked.
ClockTime makeTime(int hour, int minute)
{
    ClockTime time = {hour, minute};
    return time;
}

// TODO 2: isValidTime
//   Parameters: t -- a time (a copy; the caller's is unchanged).
//   Returns:    true if t.hour is between 0 and HOURS_PER_DAY - 1 and
//               t.minute is between 0 and MINUTES_PER_HOUR - 1; false
//               otherwise.
bool isValidTime(ClockTime t)
{
    return t.hour >= 0 && t.hour < HOURS_PER_DAY &&
           t.minute >= 0 && t.minute < MINUTES_PER_HOUR;
}

// TODO 3: minutesBetween
//   Parameters: from, to -- two valid times on the same day.
//   Returns:    The number of minutes from from to to. The result is
//               negative when to is earlier than from, and 0 when they
//               are the same time.
int minutesBetween(ClockTime from, ClockTime to)
{
    // Convert both times to minutes since midnight before subtracting.
    return (to.hour * MINUTES_PER_HOUR + to.minute) -
           (from.hour * MINUTES_PER_HOUR + from.minute);
}

// TODO 4: billedHours
//   Parameters: minutes -- the length of a stay, in minutes.
//   Returns:    The number of hours billed for the stay: every started
//               hour counts as a full hour (1 to 60 minutes is 1 hour,
//               61 to 120 minutes is 2 hours, and so on). Returns 0 when
//               minutes is 0 or negative.
int billedHours(int minutes)
{
    if (minutes <= 0)
    {
        return 0;
    }
    // Subtracting one makes exact-hour stays bill correctly under integer division.
    return 1 + (minutes - 1) / MINUTES_PER_HOUR;
}

// ---- Vehicle --------------------------------------------------------

// TODO 5: Vehicle default constructor
//   Behavior:   Initializes a placeholder vehicle: ticket 0, entered at
//               0:00, still parked, 0 minutes parked. Must delegate to
//               Vehicle(int, ClockTime).
Vehicle::Vehicle() : Vehicle(0, makeTime(0, 0))
{
}

// TODO 6: Vehicle(int ticket, ClockTime entry)
//   Parameters: ticket -- the ticket number.
//               entry  -- the time the vehicle entered the garage.
//   Behavior:   Initializes a vehicle with that ticket number and entry
//               time that is still parked, with 0 minutes parked. If
//               ticket is not between 0 and MAX_TICKET, or entry is not
//               a valid time, prints exactly
//                   Invalid vehicle setup.
//               as one line on the screen and then ends the whole program
//               with exit status 1. Must call isValidTime.
Vehicle::Vehicle(int ticket, ClockTime entry)
    : ticket_(ticket), entryTime_(entry), exitTime_(makeTime(0, 0)),
      parked_(true), minutesParked_(0)
{
    if (ticket < 0 || ticket > MAX_TICKET || !isValidTime(entry))
    {
        std::cout << "Invalid vehicle setup." << std::endl;
        exit(1);
    }
}

// TODO 7: getTicket
//   Returns:    The vehicle's ticket number.
int Vehicle::getTicket()
{
    return ticket_;
}

// TODO 8: isParked
//   Returns:    true from construction until the first successful
//               checkOut; false afterward.
bool Vehicle::isParked()
{
    return parked_;
}

// TODO 9: checkOut(ClockTime exitTime)
//   Parameters: exitTime -- the time the vehicle leaves.
//   Behavior:   If the vehicle is parked, exitTime is a valid time, and
//               exitTime is not earlier than the entry time, records the
//               check-out: the vehicle is no longer parked and its
//               minutes parked are the minutes from the entry time to
//               exitTime. Otherwise changes nothing. Must call
//               minutesBetween.
//   Returns:    The minutes parked after a successful check-out (0 or
//               more); otherwise -1.
int Vehicle::checkOut(ClockTime exitTime)
{
    if (!parked_ || !isValidTime(exitTime))
    {
        return -1;
    }

    int minutes = minutesBetween(entryTime_, exitTime);
    if (minutes < 0)
    {
        return -1;
    }

    // Update the stay only after all check-out validation has passed.
    exitTime_ = exitTime;
    minutesParked_ = minutes;
    parked_ = false;
    return minutesParked_;
}

// TODO 10: checkOut(int hour, int minute)
//   Behavior:   Same as checkOut(makeTime(hour, minute)). Must call
//               checkOut(ClockTime).
//   Returns:    The value returned by checkOut(ClockTime).
int Vehicle::checkOut(int hour, int minute)
{
    return checkOut(makeTime(hour, minute));
}

// TODO 11: getMinutesParked
//   Returns:    The minutes parked recorded by a successful checkOut, or
//               0 while the vehicle is still parked.
int Vehicle::getMinutesParked()
{
    return minutesParked_;
}

// TODO 12: output
//   Parameters: outs -- an output stream (cout, an ofstream, or any
//                       other ostream) to write to.
//   Behavior:   Writes exactly one line to outs, in the exact format
//               given in the assignment handout. While parked:
//                   Ticket <ticket>: in <entry time>, parked
//               After check-out:
//                   Ticket <ticket>: in <entry time>, out <exit time> (<minutes> min)
//               Each time is written as the hour, a colon, and the
//               minute as two digits (for example 8:05 or 14:30). The
//               line ends with a line break. Writes nothing to cout
//               unless outs is cout.
void Vehicle::output(std::ostream& outs)
{
    outs << "Ticket " << ticket_ << ": in "
         << entryTime_.hour << ':';
    // Preserve the required two-digit minute format without padding the hour.
    if (entryTime_.minute < 10)
    {
        outs << '0';
    }
    outs << entryTime_.minute;

    if (parked_)
    {
        outs << ", parked";
    }
    else
    {
        outs << ", out " << exitTime_.hour << ':';
        if (exitTime_.minute < 10)
        {
            outs << '0';
        }
        outs << exitTime_.minute << " (" << minutesParked_ << " min)";
    }
    outs << '\n';
}

// ---- Car ------------------------------------------------------------

// TODO 13: Car default constructor
//   Behavior:   Initializes a placeholder car, as Vehicle(), that is not
//               electric.
Car::Car() : Vehicle(), electric_(false)
{
}

// TODO 14: Car(int ticket, ClockTime entry)
//   Behavior:   Initializes a car that is not electric; ticket and entry
//               are handled as in TODO 6. Must delegate to
//               Car(int, ClockTime, bool).
Car::Car(int ticket, ClockTime entry) : Car(ticket, entry, false)
{
}

// TODO 15: Car(int ticket, ClockTime entry, bool electric)
//   Behavior:   Initializes a car with that ticket and entry time
//               (handled as in TODO 6) that is electric exactly when
//               electric is true. Must pass ticket and entry to
//               Vehicle(int, ClockTime) in its initialization section.
Car::Car(int ticket, ClockTime entry, bool electric)
    : Vehicle(ticket, entry), electric_(electric)
{
}

// TODO 16: isElectric
//   Returns:    true if the car is electric; false otherwise.
bool Car::isElectric()
{
    return electric_;
}

// TODO 17: Car::getFeeCents
//   Returns:    0 while the car is still parked. After check-out:
//               CAR_RATE_CENTS for each billed hour of the stay, but
//               never more than CAR_DAILY_MAX_CENTS; an electric car
//               then pays CHARGING_FEE_CENTS in addition (the charging
//               fee is not limited by the daily maximum). Must call
//               billedHours.
int Car::getFeeCents()
{
    if (isParked())
    {
        return 0;
    }

    int fee = billedHours(getMinutesParked()) * CAR_RATE_CENTS;
    // The charging fee is added after the parking-rate cap is applied.
    if (fee > CAR_DAILY_MAX_CENTS)
    {
        fee = CAR_DAILY_MAX_CENTS;
    }
    if (electric_)
    {
        fee += CHARGING_FEE_CENTS;
    }
    return fee;
}

// ---- Motorcycle -----------------------------------------------------

// TODO 18: Motorcycle default constructor
//   Behavior:   Initializes a placeholder motorcycle, as Vehicle().
Motorcycle::Motorcycle() : Vehicle()
{
}

// TODO 19: Motorcycle(int ticket, ClockTime entry)
//   Behavior:   Initializes a motorcycle with that ticket and entry time,
//               handled as in TODO 6. Must pass ticket and entry to
//               Vehicle(int, ClockTime) in its initialization section.
Motorcycle::Motorcycle(int ticket, ClockTime entry) : Vehicle(ticket, entry)
{
}

// TODO 20: Motorcycle::getFeeCents
//   Returns:    0 while the motorcycle is still parked. After check-out:
//               MOTORCYCLE_RATE_CENTS for each billed hour of the stay,
//               but never more than MOTORCYCLE_DAILY_MAX_CENTS. Must call
//               billedHours.
int Motorcycle::getFeeCents()
{
    if (isParked())
    {
        return 0;
    }

    int fee = billedHours(getMinutesParked()) * MOTORCYCLE_RATE_CENTS;
    if (fee > MOTORCYCLE_DAILY_MAX_CENTS)
    {
        fee = MOTORCYCLE_DAILY_MAX_CENTS;
    }
    return fee;
}

// ---- Bicycle --------------------------------------------------------

// TODO 21: Bicycle default constructor
//   Behavior:   Initializes a placeholder bicycle, as Vehicle(), with
//               rack number 0.
Bicycle::Bicycle() : Vehicle(), rack_(0)
{
}

// TODO 22: Bicycle(int ticket, ClockTime entry, int rack)
//   Behavior:   Initializes a bicycle with that ticket and entry time
//               (handled as in TODO 6), locked to rack. If rack is not
//               between 1 and BICYCLE_RACKS, prints exactly
//                   Invalid vehicle setup.
//               as one line on the screen and then ends the whole program
//               with exit status 1. Must pass ticket and entry to
//               Vehicle(int, ClockTime) in its initialization section.
Bicycle::Bicycle(int ticket, ClockTime entry, int rack)
    : Vehicle(ticket, entry), rack_(rack)
{
    if (rack < 1 || rack > BICYCLE_RACKS)
    {
        std::cout << "Invalid vehicle setup." << std::endl;
        exit(1);
    }
}

// TODO 23: getRack
//   Returns:    The bicycle's rack number (0 for a placeholder).
int Bicycle::getRack()
{
    return rack_;
}

// TODO 24: Bicycle::getFeeCents
//   Returns:    0 while the bicycle is still parked. After check-out:
//               nothing for the first BICYCLE_FREE_HOURS billed hours of
//               the stay, then BICYCLE_RATE_CENTS for each further billed
//               hour. Must call billedHours.
int Bicycle::getFeeCents()
{
    if (isParked())
    {
        return 0;
    }

    int hours = billedHours(getMinutesParked());
    int chargeableHours = hours - BICYCLE_FREE_HOURS;
    // A stay within the free allowance must not produce a negative charge.
    if (chargeableHours < 0)
    {
        chargeableHours = 0;
    }
    return chargeableHours * BICYCLE_RATE_CENTS;
}
