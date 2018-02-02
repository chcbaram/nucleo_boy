/*
 *  ap.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram
 */

#include "ap.h"




cmd_t cmd_boot;


void lcdLoop(void);



void apInit(void)
{
  cmdInit(&cmd_boot);
  cmdBegin(&cmd_boot, _DEF_UART1, 115200);
}

void apMain(void)
{

  while(1)
  {
    cmdifMain();
    lcdLoop();
  }
}


void lcdLoop(void)
{
  uint16_t color = _BLUE;
  static uint16_t x = 0;
  uint32_t pre_time;

  static uint16_t x_cur = 0;
  static uint16_t y_cur = 0;
  uint16_t cur_speed = 2;


  //lcdFillScreen(_BLUE);
  //lcdDrawFrame();

  pre_time = millis();

  lcdFillScreen(_BLACK);
  lcdFillRect(x, 0, 50, 50, color);


  if (buttonGetPressed(_HW_DEF_BUTTON_UP) == true)
  {
    y_cur  = y_cur - cur_speed;
    y_cur %= (128-20);
  }
  if (buttonGetPressed(_HW_DEF_BUTTON_DOWN) == true)
  {
    y_cur  = y_cur + cur_speed;
    y_cur %= (128-20);
  }
  if (buttonGetPressed(_HW_DEF_BUTTON_LEFT) == true)
  {
    x_cur  = x_cur - cur_speed;
    x_cur %= (160-20);
  }
  if (buttonGetPressed(_HW_DEF_BUTTON_RIGHT) == true)
  {
    x_cur  = x_cur + cur_speed;
    x_cur %= (160-20);
  }

  lcdFillRect(x_cur, y_cur, 20, 20, _GREEN);

  lcdDrawFrame();
  //color += 50;

  x++;
  x %= (160-50);
}
