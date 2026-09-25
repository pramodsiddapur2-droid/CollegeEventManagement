function registerEvent(eventName) {
    document.getElementById("event").value = eventName;

    document.getElementById("register").scrollIntoView({
        behavior: "smooth"
    });
}

async function submitForm(event) {
    event.preventDefault();

    let name = document.getElementById("name").value;
    let usn = document.getElementById("usn").value;
    let email = document.getElementById("email").value;
    let eventName = document.getElementById("event").value;

    try {
        let response = await fetch(
            "http://localhost:8080/register?" +
            "name=" + encodeURIComponent(name) +
            "&usn=" + encodeURIComponent(usn) +
            "&email=" + encodeURIComponent(email) +
            "&event=" + encodeURIComponent(eventName),
            {
                method: "POST"
            }
        );

        let result = await response.text();

        document.getElementById("message").innerHTML =
            result + " " + name +
            " (" + usn + ") registered for " + eventName + ".";

        document.querySelector("form").reset();
    }
    catch (error) {
        document.getElementById("message").innerHTML =
            "Unable to connect to backend.";
    }
}