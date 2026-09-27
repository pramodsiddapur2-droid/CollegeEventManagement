#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include "../httplib.h"

using namespace std;

string jsonEscape(string text)
{
    string result;

    for (char c : text)
    {
        if (c == '"')
            result += "\\\"";
        else if (c == '\\')
            result += "\\\\";
        else
            result += c;
    }

    return result;
}

string runCurl(string command)
{
    FILE* pipe = _popen(command.c_str(), "r");

    if (!pipe)
        return "";

    char buffer[4096];
    string result;

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        result += buffer;
    }

    _pclose(pipe);

    return result;
}

int main()
{
    httplib::Server server;

    const char* key = getenv("SUPABASE_KEY");

    if (key == nullptr)
    {
        cout << "SUPABASE_KEY not found!" << endl;
        return 1;
    }

    string supabaseKey = key;

    // CORS
    server.set_pre_routing_handler(
        [](const httplib::Request& req, httplib::Response& res)
        {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.set_header(
                "Access-Control-Allow-Headers",
                "Content-Type, apikey, Authorization"
            );

            if (req.method == "OPTIONS")
            {
                res.status = 200;
                return httplib::Server::HandlerResponse::Handled;
            }

            return httplib::Server::HandlerResponse::Unhandled;
        }
    );

    // Home
    server.Get("/", [](const httplib::Request& req, httplib::Response& res)
    {
        res.set_content(
            "College Event Management Backend is Running!",
            "text/plain"
        );
    });

    // Register
    server.Post("/register",
        [&](const httplib::Request& req, httplib::Response& res)
        {
            string name = req.get_param_value("name");
            string usn = req.get_param_value("usn");
            string email = req.get_param_value("email");
            string event = req.get_param_value("event");

            string json =
                "{"
                "\"name\":\"" + jsonEscape(name) + "\","
                "\"usn\":\"" + jsonEscape(usn) + "\","
                "\"email\":\"" + jsonEscape(email) + "\","
                "\"event\":\"" + jsonEscape(event) + "\""
                "}";

            string command =
                "curl -s -X POST "
                "\"https://roymcjogipzlbfztvzcy.supabase.co/rest/v1/registrations\" "
                "-H \"apikey: " + supabaseKey + "\" "
                "-H \"Authorization: Bearer " + supabaseKey + "\" "
                "-H \"Content-Type: application/json\" "
                "-H \"Prefer: return=minimal\" "
                "--data-raw \"{\\\"name\\\":\\\"" + jsonEscape(name) +
"\\\",\\\"usn\\\":\\\"" + jsonEscape(usn) +
"\\\",\\\"email\\\":\\\"" + jsonEscape(email) +
"\\\",\\\"event\\\":\\\"" + jsonEscape(event) + "\\\"}\"";
string result = runCurl(command);

if (result.empty())
{
    res.set_content(
        "Registration Successful!",
        "text/plain"
    );
}
else
{
    res.status = 500;
    res.set_content(
        "Database Error: " + result,
        "text/plain"
    );
}
            
        }
    );

    // Get registrations
    server.Get("/registrations",
        [&](const httplib::Request& req, httplib::Response& res)
        {
            string command =
                "curl -s "
                "\"https://roymcjogipzlbfztvzcy.supabase.co/rest/v1/registrations"
                "?select=id,name,usn,email,event,registered_at&order=id.asc\" "
                "-H \"apikey: " + supabaseKey + "\" "
                "-H \"Authorization: Bearer " + supabaseKey + "\"";

            string result = runCurl(command);

            if (result.empty())
            {
                res.status = 500;
                res.set_content(
                    "Unable to fetch registrations.",
                    "text/plain"
                );
            }
            else
            {
                res.set_content(
                    result,
                    "application/json"
                );
            }
        }
    );

    cout << "==============================" << endl;
    cout << " COLLEGE EVENT MANAGEMENT" << endl;
    cout << " Supabase Backend Server" << endl;
    cout << " http://localhost:8080" << endl;
    cout << "==============================" << endl;

    server.listen("localhost", 8080);

    return 0;
}