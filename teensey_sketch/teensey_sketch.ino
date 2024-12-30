#include <IntervalTimer.h>

// settings



// calculated constants
#define ANALOG_BITS (pow(2,ANALOG_RESOLUTION) - 1)

// formulas
#define CELC_TO_FAHR(c) (c* 9.0 / 5.0 + 32.0)
#define TEMP_SENSOR_VOLTAGE_TO_CELC(v) ((v - 500.0) / 10.0)
#define ANALOG_BITS_TO_MILLIVOLTAGE(b) (TEENSY_VOLTAGE * 1000.0 * b / ANALOG_BITS)

//Sensor 
#define SENSOR_PIN 14
#define SENSOR_PERIOD_MILLIS 100 
#define SENSOR_PERIOD_MICROS SENSOR_PERIOD_MILLIS * 1000
#define SENSOR_SAMPLE_SIZE 100

IntervalTimer sensor_timer;
volatile int sensor_readings[SENSOR_SAMPLE_SIZE];

void poll_sensor() {
  static int i = 0;
  sensor_readings[i++] = analogRead(SENSOR_PIN);
  if (i == SENSOR_SAMPLE_SIZE)
    i = 0;
}

void setup_temp_sensor() {
  pinMode(SENSOR_PIN, INPUT);
  analogReadResolution(ANALOG_RESOLUTION);

  //wait for values to be initialized
  Serial.print("initializing... ");
  sensor_timer.begin(poll_sensor, SENSOR_PERIOD_MICROS);
  delay(SENSOR_PERIOD_MILLIS * SENSOR_SAMPLE_SIZE);
  Serial.println("complete");
}

float get_sensor_voltage() {
  int sum = 0;
  noInterrupts();
  for (int i=0; i<SENSOR_SAMPLE_SIZE; i++) {
    sum = sum + sensor_readings[i];
  }
  interrupts();
  return float(sum) / float(SENSOR_SAMPLE_SIZE);
}


//I/O
#define TERMINATOR '\n'
#define INPUT_SIZE 50
#define OUTPUT_SIZE 200

char input[INPUT_SIZE];
char output[OUTPUT_SIZE];

bool get_command() {
  /* get command if available and puts it into input. returns true if new command found */
  // check to see if anything is in waiting
  if (Serial.available()) {
    // check to see if buffer overflow and alert user
    if (Serial.readBytesUntil(TERMINATOR, input, INPUT_SIZE) >= INPUT_SIZE-1) {
      Serial.println("buffer overflow error");
      return false;
    }
    return true;
  }
  return false;
}

void print_temp() {
  float sensor_voltage = ANALOG_BITS_TO_MILLIVOLTAGE(get_sensor_voltage());
  float temp_celc = TEMP_SENSOR_VOLTAGE_TO_CELC(sensor_voltage);
  float temp_fahr = CELC_TO_FAHR(temp_celc);

  snprintf(output, OUTPUT_SIZE, "voltage: %.2f mV\ncelc: %.2f C\nfahr: %.2f F", sensor_voltage, temp_celc, temp_fahr);
  Serial.println(output);
}


//Temp Setting
#define MAX_TEMP 80
#define MIN_TEMP 55
int target_temp = 70;
void set_temp() {
  int temp;
  if (sscanf(input, "set %i", &temp) != 1)
    Serial.println("improper argument");
  else if (temp < MIN_TEMP || temp > MAX_TEMP)
    Serial.println("outside range");
  else {
    target_temp = temp;
    snprintf(output,  OUTPUT_SIZE, "temp set to %i", target_temp);
    Serial.println(output);
  }
}

//Main
void setup() {
  Serial.begin(9600);
  setup_temp_sensor();
}

void loop() {
  if (get_command()) {
    if (strcmp(input, "") == 0)
      Serial.println();
    else if (strcmp(input, "help") == 0)
      Serial.println("available commands:\n  help\n  temp\n");
    else if (strcmp(input, "temp") == 0)
      print_temp();
    else if (strncmp(input, "set ", 4) == 0)
      set_temp();
    else
      Serial.println("unknown command");
  }
}
