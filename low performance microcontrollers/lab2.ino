int hours = 0;
int minutes = 0;

// 24 hour format
unsigned long previousMinuteMillis = 0;
unsigned long previousDisplayMillis = 0;
const unsigned long minuteInterval = 60000; // ms so 1 min
const unsigned long displayInterval = 15000; // 15 seconds

// alarm
int alarmHour = -1;
int alarmMinute = -1;
bool alarmSet = false;
bool alarmTriggered = false;
bool alarmActive = false;

// button
const int alarmButtonPin = 2;

// starting
void setup() {
  Serial.begin(9600);

  // sets button as pin as pull up resistor
  // high when unpressed
  // low when pressed
  pinMode(alarmButtonPin, INPUT_PULLUP);

  Serial.println("Clock started. Use commands to set time and alarm.");
  Serial.println("Commands: T hh mm (set time), A hh mm (set alarm), C (clear alarm)");
}

// counting time 
void loop() {
  unsigned long currentMillis = millis();

  // read input
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    handleInput(input);
  }

  // track minutes
  if (currentMillis - previousMinuteMillis >= minuteInterval) {
    previousMinuteMillis = currentMillis;
    incrementMinute(); // adds a minute when a minute has passed
  }

  // show time every 15 seconds
  if (currentMillis - previousDisplayMillis >= displayInterval) {
    previousDisplayMillis = currentMillis;
    int seconds = (currentMillis / 1000) % 60;
    printTime(seconds);
  }

  // check alarm match
  if (alarmSet && hours == alarmHour && minutes == alarmMinute && !alarmTriggered) {
    alarmActive = true;
    alarmTriggered = true;
  }

  // alarm ringing
  if (alarmActive) {
    Serial.println("!!! BEEP !!! Press button to stop.");
  }

  // Stop alarm if button is pressed
  if (alarmActive && digitalRead(alarmButtonPin) == LOW) {
    alarmActive = false;
    Serial.println("Alarm stopped.");
  }
}

// increases minutes
void incrementMinute() {
  minutes++;
  if (minutes >= 60) {
    minutes = 0;
    hours++;
    if (hours >= 24) { // resets after 24 hour
      hours = 0;
    }
  }
  alarmTriggered = false; // reset for next time
}

// prints time
void printTime(int seconds) {
  Serial.print("Current time: ");
  if (hours < 10) Serial.print('0');
  Serial.print(hours);
  Serial.print(":");
  if (minutes < 10) Serial.print('0');
  Serial.print(minutes);
  Serial.print(":");
  if (seconds < 10) Serial.print('0');
  Serial.println(seconds);
}

// input handling
void handleInput(String cmd) {
  if (cmd.startsWith("T ")) {
    int h = cmd.substring(2, 4).toInt();
    int m = cmd.substring(5, 7).toInt();
    if (h >= 0 && h < 24 && m >= 0 && m < 60) {
      hours = h;
      minutes = m;
      Serial.print("Time set to ");
      Serial.print(hours);
      Serial.print(":");
      Serial.println(minutes);
    } else {
      Serial.println("Invalid time format.");
    }
  } else if (cmd.startsWith("A ")) {
    int h = cmd.substring(2, 4).toInt();
    int m = cmd.substring(5, 7).toInt();
    if (h >= 0 && h < 24 && m >= 0 && m < 60) {
      alarmHour = h;
      alarmMinute = m;
      alarmSet = true;
      Serial.print("Alarm set for ");
      Serial.print(alarmHour);
      Serial.print(":");
      Serial.println(alarmMinute);
    } else {
      Serial.println("Invalid time.");
    }
  } else if (cmd == "C") {
    // clear alarm
    clearAlarm();
  } else {
    Serial.println("Unknown command.");
  }
}

// Clears the alarm settings
void clearAlarm() {
  alarmHour = -1;
  alarmMinute = -1;
  alarmSet = false;
  alarmTriggered = false;
  alarmActive = false;
  Serial.println("Alarm cleared.");
}
