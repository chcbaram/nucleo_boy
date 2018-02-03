#include "orocaboy/Gamebuino-Meta.h"
#include "Engine.h"
Engine engine = Engine();

void setup() {
  gb.begin();
  engine.init();
}

void loop() {

  while (!gb.update());
  gb.display.clear();
  if (buttonGetPressedEvent(_HW_DEF_BUTTON_START)) {
    engine.clear();
    engine.init();
  }
  engine.update();
}

