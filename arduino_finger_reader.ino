#include <string.h>

#define MAX_DATA_LENGTH 100
#define NUM_FINGERS 5

// Buffer for incoming serial data
char buffer[MAX_DATA_LENGTH];
int bufferIndex = 0;

// Parsed finger extension values
float finger_values[NUM_FINGERS];

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino Finger Extension Reader Ready");
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
        Serial.print("Finger Extensions: Thumb=");
        Serial.print(finger_values[0]);
        Serial.print(", Index=");
        Serial.print(finger_values[1]);
        Serial.print(", Middle=");
        Serial.print(finger_values[2]);
        Serial.print(", Ring=");
        Serial.print(finger_values[3]);
        Serial.println(", Pinky=");
        Serial.println(finger_values[4]);
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
