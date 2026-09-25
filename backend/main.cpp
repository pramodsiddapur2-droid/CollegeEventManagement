#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "../httplib.h"

using namespace std;

int main()
{
    httplib::Server server;

    // CORS
    server.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");

        if (req.method == "OPTIONS")
        {
            res.status = 200;
            return httplib::Server::HandlerResponse::Handled;
        }

        return httplib::Server::HandlerResponse::Unhandled;
    });

    // Home / test
    server.Get("/", [](const httplib::Request& req, httplib::Response& res)
    {
        res.set_content(
            "College Event Management Backend is Running!",
            "text/plain"
        );
    });

    // Register student
    server.Post("/register", [](const httplib::Request& req, httplib::Response& res)
    {
        string name = req.get_param_value("name");
        string usn = req.get_param_value("usn");
        string event = req.get_param_value("event");
        string email = req.get_param_value("email");

        ofstream file("registrations.txt", ios::app);

        if (file.is_open())
        {
           file << name << " | "
     << usn << " | "
     << email << " | "
     << event << endl;
            file.close();

            res.set_content(
                "Registration Successful!",
                "text/plain"
            );
        }
        else
        {
            res.status = 500;
            res.set_content(
                "Unable to save registration.",
                "text/plain"
            );
        }
    });

    // Get all registrations
    server.Get("/registrations", [](const httplib::Request& req, httplib::Response& res)
    {
        ifstream file("registrations.txt");

        if (!file.is_open())
        {
            res.set_content("[]", "application/json");
            return;
        }

        string line;
        string json = "[";

        bool first = true;

        while (getline(file, line))
        {
            stringstream ss(line);

           string name, usn, email, event;

            getline(ss, name, '|');
            getline(ss, usn, '|');
            getline(ss, email, '|');
            getline(ss, event);

            // Remove extra spaces
            if (!name.empty() && name.front() == ' ')
    name.erase(0, 1);

if (!name.empty() && name.back() == ' ')
    name.pop_back();

if (!usn.empty() && usn.front() == ' ')
    usn.erase(0, 1);

if (!usn.empty() && usn.back() == ' ')
    usn.pop_back();

if (!email.empty() && email.front() == ' ')
    email.erase(0, 1);

if (!email.empty() && email.back() == ' ')
    email.pop_back();

if (!event.empty() && event.front() == ' ')
    event.erase(0, 1);

if (!event.empty() && event.back() == ' ')
    event.pop_back();

            if (!first)
                json += ",";

           json += "{\"name\":\"" + name +
        "\",\"usn\":\"" + usn +
        "\",\"email\":\"" + email +
        "\",\"event\":\"" + event + "\"}";
            first = false;
        }

        json += "]";

        res.set_content(json, "application/json");

        file.close();
    });

    cout << "==============================" << endl;
    cout << " COLLEGE EVENT MANAGEMENT" << endl;
    cout << " Backend Server Started" << endl;
    cout << " http://localhost:8080" << endl;
    cout << "==============================" << endl;

    server.listen("localhost", 8080);

    return 0;
}