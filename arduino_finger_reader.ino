#include <string.h>
#include <Servo.h>

#define MAX_DATA_LENGTH 100
#define NUM_FINGERS 5

constexpr int ranges[NUM_FINGERS][4] = {
  {180, 110, 180, 0},
  {180, 90, 180, 0},
  {180, 90, 180, 0},
  {180, 90, 0, 180},
  {180, 90, 0, 180}
};

// Buffer for incoming serial data
char buffer[MAX_DATA_LENGTH];
int bufferIndex = 0;

// Parsed finger extension values
float finger_values[NUM_FINGERS];

Servo servos[NUM_FINGERS];

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino Finger Extension Reader Ready");

  for(int i = 0; i<NUM_FINGERS; i++) {
    pinMode(9+i, OUTPUT);
    servos[i].attach(9+i);
    servos[i].write(ranges[i][3]);
  }
  //while(1);

  for(int i = 0; i<NUM_FINGERS; i++) {
    servos[i].write(ranges[i][2]);
    delay(1000);
    servos[i].write(ranges[i][3]);
    delay(1000);
  }

}

void loop() {
  // Read serial data into buffer
  while (Serial.available()) {
    char c = Serial.read();
    
    if (c == '\n') {
      // End of message - process the data
      buffer[bufferIndex] = '\0';
      
      // Check if this is a valid finger extension message (starts with F,)
      if (strncmp(buffer, "F,", 2) == 0) {
        // Parse comma-separated values after "F,"
        char* valueStr = buffer + 2;  // Skip "F,"
        
        for (int i = 0; i < NUM_FINGERS; i++) {
          // Find the next comma or end of string
          char* commaPos = strchr(valueStr, ',');
          if (commaPos != NULL) {
            *commaPos = '\0';
            finger_values[i] = atof(valueStr);
            valueStr = commaPos + 1;
          } else {
            // Last value - no more commas
            finger_values[i] = atof(valueStr);
            break;
          }
        }
        
        // Print parsed values for verification
        Serial.print("Thumb=");
        Serial.print(finger_values[0]);
        Serial.print(", Index=");
        Serial.print(finger_values[1]);
        Serial.print(", Middle=");
        Serial.print(finger_values[2]);
        Serial.print(", Ring=");
        Serial.print(finger_values[3]);
        Serial.println(", Pinky=");
        Serial.println(finger_values[4]);

        if(finger_values[2] > 135 && finger_values[1] < 135 && finger_values[3] < 135 && finger_values[4] < 135)
          finger_values[2] = max(finger_values[1], max(finger_values[3], finger_values[4]));

        for(int i = 0; i<NUM_FINGERS; i++) {
          int angle = map(finger_values[i],ranges[i][0], ranges[i][1], ranges[i][3], ranges[i][2]);
          servos[i].write(angle);
        }

      }
      
      // Clear buffer for next message
      bufferIndex = 0;
      memset(buffer, 0, MAX_DATA_LENGTH);
    } else if (bufferIndex < MAX_DATA_LENGTH - 1) {
      // Store character in buffer
      buffer[bufferIndex] = c;
      bufferIndex++;
    }
  }
}
