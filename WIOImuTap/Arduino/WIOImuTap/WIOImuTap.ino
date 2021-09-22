#include <LIS3DHTR.h>
#include <TFT_eSPI.h>
#include "Free_Fonts.h"

TFT_eSPI tft;
LIS3DHTR<TwoWire> lis;
uint32_t roll1, roll2, lastRoll = 0;
bool dieRolled = false;
volatile bool shakeFlag = false;

#define THRESHOLD 40 //Adjust this threshold value for sensitivity of clicking

void trngInit(void) // Initialize the TRNG
{
  MCLK->APBCMASK.bit.TRNG_ = 1; // enable clock  (APBC clock is already enabled)
  TRNG->CTRLA.bit.ENABLE = 1;   // enable the TRNG
}

uint32_t trngGetRandomNumber(void) // Waits for the next random number and returns it
{
  while ((TRNG->INTFLAG.reg & TRNG_INTFLAG_MASK) == 0)
    ; // Busy loop waiting for next RN to be created
  return (TRNG->DATA.reg);
}

void drawDie(uint32_t xoff, uint32_t yoff, uint32_t roll) // Draws the die
{
  tft.fillRoundRect(xoff, yoff, 100, 100, 10, TFT_WHITE); //die background
  switch (roll)
  {
  case 1:
    tft.fillCircle(xoff + 50, yoff + 50, 10, TFT_BLACK); //1
    break;
  case 2:
    tft.fillCircle(xoff + 30, yoff + 70, 10, TFT_BLACK); //2
    tft.fillCircle(xoff + 70, yoff + 30, 10, TFT_BLACK); //2
    break;
  case 3:
    tft.fillCircle(xoff + 50, yoff + 50, 10, TFT_BLACK); //3
    tft.fillCircle(xoff + 20, yoff + 80, 10, TFT_BLACK); //3
    tft.fillCircle(xoff + 80, yoff + 20, 10, TFT_BLACK); //3
    break;
  case 4:
    tft.fillCircle(xoff + 30, yoff + 70, 10, TFT_BLACK); //4
    tft.fillCircle(xoff + 70, yoff + 30, 10, TFT_BLACK); //4
    tft.fillCircle(xoff + 30, yoff + 30, 10, TFT_BLACK); //4
    tft.fillCircle(xoff + 70, yoff + 70, 10, TFT_BLACK); //4
    break;
  case 5:
    tft.fillCircle(xoff + 50, yoff + 50, 10, TFT_BLACK); //5
    tft.fillCircle(xoff + 20, yoff + 80, 10, TFT_BLACK); //5
    tft.fillCircle(xoff + 80, yoff + 20, 10, TFT_BLACK); //5
    tft.fillCircle(xoff + 20, yoff + 20, 10, TFT_BLACK); //5
    tft.fillCircle(xoff + 80, yoff + 80, 10, TFT_BLACK); //5
    break;
  case 6:
    tft.fillCircle(xoff + 25, yoff + 25, 10, TFT_BLACK); //6
    tft.fillCircle(xoff + 25, yoff + 50, 10, TFT_BLACK); //6
    tft.fillCircle(xoff + 25, yoff + 75, 10, TFT_BLACK); //6
    tft.fillCircle(xoff + 75, yoff + 25, 10, TFT_BLACK); //6
    tft.fillCircle(xoff + 75, yoff + 50, 10, TFT_BLACK); //6
    tft.fillCircle(xoff + 75, yoff + 75, 10, TFT_BLACK); //6
  }
}

void gen2RandomNumbers(void) // Generates 2 random numbers
{
  shakeFlag = true;
  roll1 = (trngGetRandomNumber() % 6) + 1;
  roll2 = (trngGetRandomNumber() % 6) + 1;
  // Serial.print("Tap Random: ");
  // Serial.printf("%d %d Sum: %d\n", roll1, roll2, roll1+roll2);
  lastRoll = millis();
  shakeFlag = false;
  dieRolled = true;
}

void dieRolledCallback(void) // Draws die values on Screen
{
  tft.fillScreen(TFT_BLACK);
  drawDie(40, 70, roll1);
  drawDie(180, 70, roll2);
}

void setup(void)
{
  Serial.begin(115200);
  trngInit();

  lis.begin(Wire1);
  if (!lis)
  {
    Serial.println("ERROR");
    while (1)
      ;
  }
  lis.setOutputDataRate(LIS3DHTR_DATARATE_100HZ);
  lis.setFullScaleRange(LIS3DHTR_RANGE_4G);
  lis.click(2, THRESHOLD);
  attachInterrupt(digitalPinToInterrupt(GYROSCOPE_INT1), gen2RandomNumbers, RISING);

  // Turn on TFT and display splash screen
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBoldOblique24pt7b);
  tft.drawString("SHAKE to", 50, 70);
  tft.drawString("Roll die", 50, 120);
}

void loop(void)
{
  if (!shakeFlag && dieRolled && (millis() - lastRoll) > 2000)
  {
    dieRolledCallback();
    dieRolled = false;
  }
}