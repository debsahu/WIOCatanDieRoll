#include "wio-shake-lift_inferencing.h"
#include <LIS3DHTR.h>

#define CONVERT_G_TO_MS2 9.80665f

static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal

LIS3DHTR<TwoWire> IMU;

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
    delay(5000);
    Serial.println("Edge Impulse Inferencing Demo");

    IMU.begin(Wire1);
    if (!IMU)
    {
        Serial.println("ERROR");
        while (1)
        ;
    }
    IMU.setOutputDataRate(LIS3DHTR_DATARATE_100HZ);
    // IMU.setFullScaleRange(LIS3DHTR_RANGE_4G);
    IMU.setHighSolution(true);

    if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 3)
    {
        ei_printf("ERR: EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME should be equal to 3 (the 3 sensor axes)\n");
        return;
    }
}

void loop()
{

    ei_printf("Sampling...\n");

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
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
              result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf(": \n");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
        Serial.print(result.classification[ix].label);
        Serial.print(": ");
        Serial.print(100 * result.classification[ix].value);
        Serial.println("%");
    }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_ACCELEROMETER
#error "Invalid model for current sensor"
#endif
