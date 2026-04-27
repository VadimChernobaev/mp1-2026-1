#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

enum class Zone
{
    Regular,
    VIP
};

struct Seat
{
    int row;
    int number;
    Zone zone;
    bool occupied = false;
};

struct Ticket
{
    string date;
    string time;
    string movie;
    int hall;
    int row;
    int seat;
};

struct Session
{
    string date;
    string time;
    string movie;
    int hall;
    double basePrice;

    vector<vector<Seat>> seats;
};

class Cinema
{
private:
    vector<Session> sessions;

public:

    void addSession(Session s)
    {
        sessions.push_back(s);
    }

    Session* findSession(string date, string time, string movie, int hall)
    {
        for (auto& s : sessions)
        {
            if (s.date == date &&
                s.time == time &&
                s.movie == movie &&
                s.hall == hall)
                return &s;
        }

        return nullptr;
    }

    bool checkSeats(Session* session, Zone zone, int count)
    {
        int freeSeats = 0;

        for (auto& row : session->seats)
        {
            for (auto& seat : row)
            {
                if (!seat.occupied && seat.zone == zone)
                    freeSeats++;
            }
        }

        return freeSeats >= count;
    }

    vector<Seat*> reserveSeats(Session* session, Zone zone, int count)
    {
        vector<Seat*> result;

        for (auto& row : session->seats)
        {
            for (auto& seat : row)
            {
                if (!seat.occupied && seat.zone == zone)
                {
                    seat.occupied = true;
                    result.push_back(&seat);

                    if (result.size() == count)
                        return result;
                }
            }
        }

        return result;
    }

    void releaseSeats(vector<Seat*> seats)
    {
        for (auto s : seats)
            s->occupied = false;
    }

    double calculatePrice(Session* session, Zone zone)
    {
        int hour = stoi(session->time.substr(0, 2));

        double price = session->basePrice;

        if (hour < 12)
            price *= 0.75;
        else if (hour > 18)
            price *= 1.5;

        if (zone == Zone::VIP)
            price *= 2;

        return price;
    }
};

class TicketOffice
{
private:
    Cinema& cinema;

public:

    TicketOffice(Cinema& c) : cinema(c) {}

    vector<Ticket> buyTickets(
        string date,
        string time,
        string movie,
        int hall,
        Zone zone,
        int count,
        double& totalPrice)
    {
        Session* session = cinema.findSession(date, time, movie, hall);

        if (!session)
            throw runtime_error("Session not found");

        if (!cinema.checkSeats(session, zone, count))
            throw runtime_error("Not enough seats");

        auto seats = cinema.reserveSeats(session, zone, count);

        double price = cinema.calculatePrice(session, zone);
        totalPrice = price * count;

        vector<Ticket> tickets;

        for (auto s : seats)
        {
            Ticket t;

            t.date = date;
            t.time = time;
            t.movie = movie;
            t.hall = hall;
            t.row = s->row;
            t.seat = s->number;

            tickets.push_back(t);
        }

        return tickets;
    }

    void cancelOrder(vector<Seat*> seats)
    {
        cinema.releaseSeats(seats);
    }
};

Session createSession(
    string date,
    string time,
    string movie,
    int hall,
    int rows,
    int seatsPerRow,
    double basePrice)
{
    Session s;

    s.date = date;
    s.time = time;
    s.movie = movie;
    s.hall = hall;
    s.basePrice = basePrice;

    for (int r = 0; r < rows; r++)
    {
        vector<Seat> row;

        for (int c = 0; c < seatsPerRow; c++)
        {
            Seat seat;

            seat.row = r + 1;
            seat.number = c + 1;

            if (r < 2)
                seat.zone = Zone::VIP;
            else
                seat.zone = Zone::Regular;

            row.push_back(seat);
        }

        s.seats.push_back(row);
    }

    return s;
}

int main()
{
    Cinema cinema;

    cinema.addSession(
        createSession(
            "20.04.2026",
            "19:00",
            "Project 'Hail Mary' ",
            1,
            5,
            10,
            10.0
        )
    );

    TicketOffice office(cinema);

    try
    {
        double price;

        auto tickets =
            office.buyTickets(
                "20.04.2026",
                "19:00",
                "Project 'Hail Mary' ",
                1,
                Zone::Regular,
                3,
                price
            );

        cout << "Total price: " << price << endl;

        cout << "Tickets:" << endl;

        for (auto& t : tickets)
        {
            cout
                << t.movie << " "
                << t.date << " "
                << t.time
                << " hall " << t.hall
                << " row " << t.row
                << " seat " << t.seat
                << endl;
        }
    }
    catch (exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}