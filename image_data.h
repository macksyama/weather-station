#pragma once
#include <Adafruit_GFX.h>

struct CImage {
    const uint16_t size;
    const uint16_t width;
    const uint16_t height;
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
extern const CImage image_weather_icon_100;
extern const CImage image_weather_icon_101;
extern const CImage image_weather_icon_101_s;
extern const CImage image_weather_icon_102;
extern const CImage image_weather_icon_102_s;
extern const CImage image_weather_icon_103;
extern const CImage image_weather_icon_103_s;
extern const CImage image_weather_icon_104;
extern const CImage image_weather_icon_111;
extern const CImage image_weather_icon_111_s;
extern const CImage image_weather_icon_111_t;
extern const CImage image_weather_icon_114;
extern const CImage image_weather_icon_114_s;
extern const CImage image_weather_icon_114_t;
extern const CImage image_weather_icon_117;
extern const CImage image_weather_icon_200;
extern const CImage image_weather_icon_201;
extern const CImage image_weather_icon_201_s;
extern const CImage image_weather_icon_202;
extern const CImage image_weather_icon_202_s;
extern const CImage image_weather_icon_203;
extern const CImage image_weather_icon_203_s;
extern const CImage image_weather_icon_204;
extern const CImage image_weather_icon_211;
extern const CImage image_weather_icon_211_s;
extern const CImage image_weather_icon_214;
extern const CImage image_weather_icon_214_s;
extern const CImage image_weather_icon_217;
extern const CImage image_weather_icon_300;
extern const CImage image_weather_icon_301;
extern const CImage image_weather_icon_301_s;
extern const CImage image_weather_icon_302;
extern const CImage image_weather_icon_302_s;
extern const CImage image_weather_icon_303;
extern const CImage image_weather_icon_306;
extern const CImage image_weather_icon_311;
extern const CImage image_weather_icon_311_s;
extern const CImage image_weather_icon_311_t;
extern const CImage image_weather_icon_313;
extern const CImage image_weather_icon_313_s;
extern const CImage image_weather_icon_313_t;
extern const CImage image_weather_icon_315;
extern const CImage image_weather_icon_400;
extern const CImage image_weather_icon_401;
extern const CImage image_weather_icon_402;
extern const CImage image_weather_icon_403;
extern const CImage image_weather_icon_407;
extern const CImage image_weather_icon_409;
extern const CImage image_weather_icon_411;
extern const CImage image_weather_icon_413;
extern const CImage image_weather_icon_414;


void cImage_write(const CImage* img, int16_t x, int16_t y, uint16_t color);
void cImage_writeSlide(const CImage* img, int16_t x, int16_t y, uint16_t color, uint8_t offset);
