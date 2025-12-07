#include <Arduino.h>
#include "SystemController.h"

SystemController controller;

void setup() {
  controller.begin();
}

void loop() {
  controller.run();
}
