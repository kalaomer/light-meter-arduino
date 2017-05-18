#include "Arduino.h"
#include "Wire.h"
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#define DISPLAY_ADDRESS 0x3C
#define APERTURE_PIN A0
#define ASA 200

float APERTURE_LIST[] = {1.8, 2.8, 4, 5.6, 8, 11, 16, 22};


void setupSensor() {
    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);

    /* Update these values depending on what you've set above! */
    Serial.println("------------------------------------");
    Serial.print  ("Gain:         "); Serial.println("Auto");
    Serial.print  ("Timing:       "); Serial.println("13 ms");
    Serial.println("------------------------------------");
}


void setup() {
    Serial.begin(9600);

    if (!tsl.begin()) {
        Serial.println("There is no light senser!");
        while(1);
    } else {
        Serial.println("Light sensor detected!");
    }

    setupSensor();

    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Loading\n");
    display.display();
    delay(2000);
}

float getApertureValue() {
    int val = analogRead(APERTURE_PIN);

    long index = map((long) val, 0, 1023, 0, 7);

    return APERTURE_LIST[(int) index];
}

float calculateExposureValue(float lux) {
    float k = log(ASA) / log(100);
    
    return (log(lux/2.5) / log(2)) + k;
}

float calculateShutterSpeed(float lux) {
    float aperturealue = getApertureValue();

    float shutterSpeed = (float) square((double) aperturealue) / pow(2, calculateExposureValue(lux));

    return shutterSpeed;
}

void loop() {
    sensors_event_t event;
    tsl.getEvent(&event);

    if (event.light)
    {
        float lux = event.light;

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print(lux); display.println(" lux");
        display.print(getApertureValue()); display.println(" f");
        display.print(calculateExposureValue(lux)); display.println(" EV");
        display.print(calculateShutterSpeed(lux)); display.println("s shutter");
        display.display();

        Serial.println("------------------------------------");
        Serial.print(lux); Serial.println(" lux");
        Serial.print(getApertureValue()); Serial.println(" f");
        Serial.print(calculateExposureValue(lux)); Serial.println(" EV");
        Serial.print(calculateShutterSpeed(lux)); Serial.println("s shutter");

    }
    else
    {
        Serial.println("Sensor overload");
    }

    delay(200);
}
