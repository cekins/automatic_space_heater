#include <SimpleCLI.h>

/* PINS */

#define TEMP_PIN 14

char terminator = '\n';

SimpleCLI cli;



Command help_cmd = cli.addCmd("help");
Command set_cmd = cli.addCmd("set");
Command get_cmd = cli.addCmd("get");

int goal_temp = 70;

void setup_temp_sensor() {

}

void get_temp() {
  float sensor_voltage = analogRead(TEMP_PIN) * 3.3 / 1023.0 * 1000.0;
  float temp_celc = (sensor_voltage - 500.0) / 10.0;
  float temp_fahr = temp_celc * 9.0 / 5.0 + 32.0;
  Serial.println("voltage: " + String(sensor_voltage) + "\ncelc: " + String(temp_celc) + "\nfahr: " + String(temp_fahr));
}

void setup() {
  pinMode(TEMP_PIN, INPUT);
  Serial.begin(9600);

}

void loop() {
  String input = Serial.readString();
  cli.parse(input);
  if(cli.available()) {
    Command cmd = cli.getCommand();

    if(cmd == help_cmd) {
      Serial.println("this is the help command");
    }
    if(cmd == get_cmd) {
      get_temp();
    }
  }
}
