#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <AccelStepper.h>
#include <time.h>
#include "secrets.h"

// ================= WIFI =================

WebServer server(80);


// ================= STEPPER =================

#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

AccelStepper stepper(
    AccelStepper::HALF4WIRE,
    IN1,
    IN3,
    IN2,
    IN4
);

const int STEPS_45 = 512;

// Tracks whether the driver outputs are currently energized.
// Used so we only disable outputs once, right after a move finishes.
bool motorEnergized = false;


// ================= CLOCK =================

// Dallas Texas time zone
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -6 * 3600;
const int daylightOffset_sec = 3600;


// ================= SCHEDULE =================

int scheduledHour = -1;
int scheduledMinute = -1;

bool alreadyRotatedToday = false;


// ================= DAY TRACKING =================

// 8 physical positions on the dispenser (45 deg apart, 8 x 45 = 360):
// Monday - Sunday, plus one Refill slot.
const char* dayNames[8] = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday",
    "Refill"
};

// Index into dayNames representing which position the dispenser
// is currently on. Defaults to Monday until set.
int currentDayIndex = 0;


// ================= MOTOR FUNCTIONS =================

void rotateCW()
{
    stepper.enableOutputs();
    motorEnergized = true;

    stepper.moveTo(stepper.currentPosition() - STEPS_45);   // was +

    currentDayIndex = (currentDayIndex + 1) % 8;
}

void rotateCCW()
{
    stepper.enableOutputs();
    motorEnergized = true;

    stepper.moveTo(stepper.currentPosition() + STEPS_45);   // was -

    currentDayIndex = (currentDayIndex - 1 + 8) % 8;
}


// ================= WEB PAGE =================

void handleRoot()
{
    struct tm timeinfo;

    char currentTime[20] = "--:--:--";

    if (getLocalTime(&timeinfo))
    {
        strftime(
            currentTime,
            sizeof(currentTime),
            "%I:%M:%S %p",
            &timeinfo
        );
    }

    // Value for the native <input type="time"> field, e.g. "07:30"
    char scheduleValue[8] = "";
    if (scheduledHour >= 0)
    {
        snprintf(
            scheduleValue,
            sizeof(scheduleValue),
            "%02d:%02d",
            scheduledHour,
            scheduledMinute
        );
    }

    char scheduleLabel[24] = "Not set";
    if (scheduledHour >= 0)
    {
        struct tm t = {};
        t.tm_hour = scheduledHour;
        t.tm_min = scheduledMinute;
        strftime(scheduleLabel, sizeof(scheduleLabel), "%I:%M %p", &t);
    }

    const char* currentDayLabel = dayNames[currentDayIndex];

    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
<title>Dispenser Control</title>
<style>
  :root {
    --bg: #0f1115;
    --card: #1a1d24;
    --accent: #4f8ff7;
    --accent-dim: #2c3f63;
    --text: #f0f2f5;
    --muted: #8a90a0;
    --success: #3ecf8e;
    --radius: 16px;
  }

  * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }

  body {
    margin: 0;
    padding: 20px 16px 40px;
    background: var(--bg);
    color: var(--text);
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
    display: flex;
    justify-content: center;
  }

  .wrap {
    width: 100%;
    max-width: 420px;
  }

  header {
    text-align: center;
    margin-bottom: 22px;
  }

  header h1 {
    font-size: 20px;
    font-weight: 600;
    margin: 0 0 4px;
    letter-spacing: 0.2px;
  }

  header .sub {
    color: var(--muted);
    font-size: 13px;
  }

  .card {
    background: var(--card);
    border-radius: var(--radius);
    padding: 20px;
    margin-bottom: 16px;
    box-shadow: 0 1px 0 rgba(255,255,255,0.03) inset;
  }

  .card h2 {
    font-size: 13px;
    text-transform: uppercase;
    letter-spacing: 0.8px;
    color: var(--muted);
    margin: 0 0 14px;
    font-weight: 600;
  }

  .clock {
    font-size: 40px;
    font-weight: 700;
    text-align: center;
    letter-spacing: 1px;
    font-variant-numeric: tabular-nums;
  }

  .clock-sub {
    text-align: center;
    color: var(--muted);
    font-size: 12px;
    margin-top: 6px;
  }

  .btn-row {
    display: flex;
    gap: 12px;
  }

  .btn {
    flex: 1;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 4px;
    padding: 18px 8px;
    border-radius: 12px;
    border: none;
    background: var(--accent-dim);
    color: var(--text);
    font-size: 15px;
    font-weight: 600;
    text-decoration: none;
    transition: transform 0.08s ease, background 0.15s ease;
  }

  .btn:active {
    transform: scale(0.96);
    background: var(--accent);
  }

  .btn .icon {
    font-size: 22px;
  }

  .schedule-row {
    display: flex;
    gap: 10px;
    align-items: center;
  }

  input[type="time"] {
    flex: 1;
    font-size: 18px;
    padding: 12px 10px;
    border-radius: 10px;
    border: 1px solid #2a2e38;
    background: #12141a;
    color: var(--text);
    font-family: inherit;
  }

  .save-btn {
    padding: 12px 18px;
    border-radius: 10px;
    border: none;
    background: var(--accent);
    color: #fff;
    font-size: 15px;
    font-weight: 600;
  }

  .save-btn:active {
    transform: scale(0.96);
  }

  .status-line {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-top: 14px;
    font-size: 13px;
    color: var(--muted);
  }

  .status-line .value {
    color: var(--success);
    font-weight: 600;
  }

  footer {
    text-align: center;
    color: var(--muted);
    font-size: 11px;
    margin-top: 8px;
  }
</style>
</head>
<body>
<div class="wrap">

  <header>
    <h1>Dispenser Control</h1>
    <div class="sub">ESP32 &middot; Wi-Fi connected</div>
  </header>

  <div class="card">
    <h2>Current Time</h2>
    <div class="clock" id="clock">)rawliteral";

    html += currentTime;

    html += R"rawliteral(</div>
    <div class="clock-sub">Dallas, TX (auto-synced)</div>
  </div>

  <div class="card">
    <h2>Dispenser Day</h2>
    <div class="clock" id="day" style="font-size:28px;">)rawliteral";

    html += currentDayLabel;

    html += R"rawliteral(</div>
    <div class="clock-sub">Currently set to this day</div>
  </div>

  <div class="card">
    <h2>Manual Rotation</h2>
    <div class="btn-row">
      <a class="btn" href="/cw">
        <span class="icon">&#8635;</span>
        <span>One Day Forward</span>
      </a>
      <a class="btn" href="/ccw">
        <span class="icon">&#8634;</span>
        <span>One Day Backward</span>
      </a>
    </div>
  </div>

  <div class="card">
    <h2>Correct Current Day</h2>
    <form action="/setday" method="GET">
      <div class="schedule-row">
        <select name="day" style="flex:1; font-size:16px; padding:12px 10px; border-radius:10px; border:1px solid #2a2e38; background:#12141a; color:#f0f2f5; font-family:inherit;">
)rawliteral";

    for (int i = 0; i < 8; i++)
    {
        html += "<option value=\"";
        html += i;
        html += "\"";
        if (i == currentDayIndex) html += " selected";
        html += ">";
        html += dayNames[i];
        html += "</option>";
    }

    html += R"rawliteral(
        </select>
        <button class="save-btn" type="submit">Set</button>
      </div>
    </form>
  </div>

  <div class="card">
    <h2>Daily Rotation Schedule</h2>
    <form action="/schedule" method="GET">
      <div class="schedule-row">
        <input type="time" name="time" value=")rawliteral";

    html += scheduleValue;

    html += R"rawliteral(" required>
        <button class="save-btn" type="submit">Save</button>
      </div>
    </form>
    <div class="status-line">
      <span>Scheduled for</span>
      <span class="value">)rawliteral";

    html += scheduleLabel;

    html += R"rawliteral(</span>
    </div>
  </div>

  <footer>Rotation runs automatically once per day at the scheduled time.</footer>

</div>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
}


// ================= WEB BUTTONS =================


void handleCW()
{
    rotateCW();

    server.sendHeader("Location", "/");
    server.send(303);
}


void handleCCW()
{
    rotateCCW();

    server.sendHeader("Location", "/");
    server.send(303);
}



void handleSetDay()
{
    if (server.hasArg("day"))
    {
        int d = server.arg("day").toInt();

        if (d >= 0 && d <= 7)
        {
            currentDayIndex = d;
        }
    }

    server.sendHeader("Location", "/");
    server.send(303);
}



void handleSchedule()
{
    // Expects a "time" arg from <input type="time">, formatted as "HH:MM"
    if (server.hasArg("time"))
    {
        String t = server.arg("time");

        int colonIndex = t.indexOf(':');

        if (colonIndex > 0)
        {
            scheduledHour = t.substring(0, colonIndex).toInt();
            scheduledMinute = t.substring(colonIndex + 1).toInt();
            alreadyRotatedToday = false;
        }
    }

    server.sendHeader("Location", "/");
    server.send(303);
}



// ================= SETUP =================

void setup()
{
    Serial.begin(115200);


    stepper.setMaxSpeed(600);
    stepper.setAcceleration(300);

    // Start with coils de-energized until a move is requested.
    stepper.disableOutputs();
    motorEnergized = false;



    Serial.println("Connecting WiFi...");

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );


    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }


    Serial.println();
    Serial.println("WiFi Connected");


    Serial.print("IP Address: ");
    Serial.println(
        WiFi.localIP()
    );


    configTime(
        gmtOffset_sec,
        daylightOffset_sec,
        ntpServer
    );


    Serial.println("Waiting for time");


    struct tm timeinfo;

    while(!getLocalTime(&timeinfo))
    {
        delay(500);
        Serial.print(".");
    }


    Serial.println();
    Serial.println("Clock Ready");



    server.on("/", handleRoot);

    server.on("/cw", handleCW);

    server.on("/ccw", handleCCW);

    server.on("/schedule",
              handleSchedule);

    server.on("/setday",
              handleSetDay);


    server.begin();


    Serial.println("Web server started");

}



// ================= LOOP =================

void loop()
{

    server.handleClient();


    stepper.run();

    // Once a move finishes, cut power to the coils so the motor
    // and ULN2003 aren't sitting there energized (and warm) all day.
    if (motorEnergized && stepper.distanceToGo() == 0)
    {
        stepper.disableOutputs();
        motorEnergized = false;
    }



    struct tm timeinfo;


    if(getLocalTime(&timeinfo))
    {

        if(
          scheduledHour == timeinfo.tm_hour &&
          scheduledMinute == timeinfo.tm_min &&
          !alreadyRotatedToday
        )
        {

            Serial.println(
                "Scheduled rotation!"
            );


            rotateCW();


            alreadyRotatedToday = true;

        }



        // Reset after midnight

        if(timeinfo.tm_hour == 0 &&
           timeinfo.tm_min == 0)
        {
            alreadyRotatedToday = false;
        }

    }

}