struct scc_appliances
{
  char locations[4];
  int pins[5];
  byte states[5];
};

struct scc_appliances appliances[10] =
{
  {"F1L", {D0}, {0}},
  {"F2L", {D1}, {0}},
  {"F3L", {D2}, {0}},
  {"F4L", {D3}, {0}},
  {"F1F", {D4}, {0}},
  {"F2F", {D5}, {0}},
  {"F3F", {D5}, {0}},
  {"F4F", {D6}, {0}},
  {"Z1L", {D0, D1, D2, D3}, {0, 0, 0, 0}},
  {"Z1F", {D4, D5, D6, D7}, {0, 0, 0, 0}},
};

