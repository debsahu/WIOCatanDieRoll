#include <LIS3DHTR.h>
#include <TFT_eSPI.h>
#include "Free_Fonts.h"
#include "wio-shake-lift_inferencing.h"

#define CONVERT_G_TO_MS2 9.80665f

static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal

LIS3DHTR<TwoWire> IMU;
TFT_eSPI tft;
uint32_t roll1, roll2, lastRoll = 0;
bool dieRolled = false;
volatile bool shakeFlag = false;

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

void ei_printf(const char *format, ...)
{
    static char print_buf[1024] = {0};

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0)
    {
        Serial.write(print_buf);
    }
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    trngInit();

    // Turn on IMU
    IMU.begin(Wire1);
    if (!IMU)
    {
        Serial.println("ERROR");
        while (1)
            ;
    }
    IMU.setOutputDataRate(LIS3DHTR_DATARATE_100HZ);
    IMU.setHighSolution(true);

    if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 3)
    {
        ei_printf("ERR: EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME should be equal to 3 (the 3 sensor axes)\n");
        return;
    }

    // Turn on TFT and display splash screen
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(&FreeSansBoldOblique24pt7b);
    tft.drawString("SHAKE to", 50, 70);
    tft.drawString("Roll die", 50, 120);
}

void loop()
{

    if (!shakeFlag && dieRolled && (millis() - lastRoll) > 1000)
    {
        dieRolledCallback();
        dieRolled = false;
    }
    // ei_printf("Sampling...\n");

    // Allocate a buffer here for the values we'll read from the IMU
    float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {0};

    for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 3)
    {
        // Determine the next tick (and then sleep later)
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        buffer[ix + 0] = IMU.getAccelerationX();
        buffer[ix + 1] = IMU.getAccelerationY();
        buffer[ix + 2] = IMU.getAccelerationZ();

        buffer[ix + 0] *= CONVERT_G_TO_MS2;
        buffer[ix + 1] *= CONVERT_G_TO_MS2;
        buffer[ix + 2] *= CONVERT_G_TO_MS2;

        delayMicroseconds(next_tick - micros());
    }

    // Turn the raw buffer in a signal which we can the classify
    signal_t signal;
    int err = numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    if (err != 0)
    {
        ei_printf("Failed to create signal from buffer (%d)\n", err);
        return;
    }

    // Run the classifier
    ei_impulse_result_t result = {0};

    err = run_classifier(&signal, &result, debug_nn);
    if (err != EI_IMPULSE_OK)
    {
        ei_printf("ERR: Failed to run classifier (%d)\n", err);
        return;
    }

    // print the predictions
    // ei_printf("Predictions ");
    // ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
    //           result.timing.dsp, result.timing.classification, result.timing.anomaly);
    // ei_printf(": \n");
    // for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    // {
    //     Serial.print(result.classification[ix].label);
    //     Serial.print(": ");
    //     Serial.print(100 * result.classification[ix].value);
    //     Serial.println("%");
    // }

    if (result.classification[2].value > 0.75)
    {
        gen2RandomNumbers();
    }

#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_ACCELEROMETER
#error "Invalid model for current sensor"
#endif
