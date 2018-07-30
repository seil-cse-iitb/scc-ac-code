
char test[] = {"F3"};

struct _205
{
  // In 205,D0-D3 are connected to the lights and D4-D7 to the fans
  // Actuation works on negative logic i.e. To turn on, give LOW to the particular pin

  int total_fans = 4;
  int total_lights = 4;
  int appliance_pins[8] = {D0, D1, D2, D3, D4, D5, D6, D7};
  String appliances[8] = {"L1", "L2", "L3", "L4", "F1", "F2", "F3", "F4"};
  byte appliance_status[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  byte eeprom_address[8] = {0, 1, 2, 3, 4, 5, 6, 7};

} config205;
