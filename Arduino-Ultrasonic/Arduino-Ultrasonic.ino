#define echoPin 12 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 11 //attach pin D3 Arduino to pin Trig of HC-SR04
#define statusPin 5 // high if door is open, low if door is closed, NOTE: This will be 5V, esp32 needs 3.3V double check first, do not plug directly

// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

int last10Distances[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void pushDistance(int dist) {
  // Push new value into distance queue and also filter out any spikes
  const int tolerance = 500; // Any spikes larger than a 500cm difference from previous will trigger filter

  while (dist - last10Distances[9] > tolerance) {
    // If a spike is detected, get median of last good distance and the new measurement
    // and store it as the new dist. Repeat this until the spike is within the tolerance value
    dist = (dist + last10Distances[9]) / 2;
  }
  
  for (int i = 1; i < 10; ++i) {
    last10Distances[i-1] = last10Distances[i];
  }
  last10Distances[9] = dist;
}

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  pinMode(statusPin, OUTPUT);
  pinMode(13, OUTPUT); // Turn off builtin LED
  digitalWrite(13, LOW);
  //Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
}
void loop() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  pushDistance(duration * 0.034 / 2); // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(last10Distances[9]);
  Serial.println(" cm");

  if (last10Distances[9] > 10) {
    Serial.println("Door open");
    digitalWrite(statusPin, HIGH);
  } else {
    Serial.println("Door closed");
    digitalWrite(statusPin, LOW);
  }
}
