#pragma once
#include <Adafruit_GFX.h>

struct CImage {
    const uint16_t size;
    const uint16_t width;
    const uint8_t* data;
};

extern const CImage image_temperature_icon;
extern const CImage image_temperature_value;
extern const CImage image_humidity_icon;
extern const CImage image_humidity_value;
extern const CImage image_di_face1;
extern const CImage image_di_face2;
extern const CImage image_di_face3;
extern const CImage image_di_face4;

void cImage_write(const CImage* img, int16_t x, int16_t y);
void cImage_writeSlide(const CImage* img, uint8_t offset, uint16_t color, 
                      int16_t x, int16_t y);
