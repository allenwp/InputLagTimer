#include "stdafx.h"
#include "Config.h"

double Config::lowestAccuracy = 0;
double Config::highestJitter = 0;

int Config::numColumns = 2;
DirectX::XMVECTOR Config::fontColour = { 0.0, 0.0, 0.0, 1.0f };
float Config::backgroundColour[4] = { 1.0, 1.0, 1.0, 1.0f };

void Config::config()
{
  int lowestAccuracy = GetPrivateProfileInt(L"FAILSAFES", L"lowest_accuracy", 10, L".\\res\\config.ini");
  Config::lowestAccuracy = (double)lowestAccuracy / 10000.0;
  int highestJitter = GetPrivateProfileInt(L"FAILSAFES", L"highest_jitter", 10, L".\\res\\config.ini");
  Config::highestJitter = (double)highestJitter / 10000.0;
  
  Config::numColumns = GetPrivateProfileInt(L"DISPLAY", L"num_columns", 2, L".\\res\\config.ini");

  int colour = GetPrivateProfileInt(L"DISPLAY", L"font_colour_r", 255, L".\\res\\config.ini");
  float r = getColourComponent(colour);
  colour = GetPrivateProfileInt(L"DISPLAY", L"font_colour_g", 255, L".\\res\\config.ini");
  float g = getColourComponent(colour);
  colour = GetPrivateProfileInt(L"DISPLAY", L"font_colour_b", 255, L".\\res\\config.ini");
  float b = getColourComponent(colour);
  DirectX::XMVECTOR newColour = { r, g, b, 1.0f };
  fontColour = newColour;

  colour = GetPrivateProfileInt(L"DISPLAY", L"background_colour_r", 0, L".\\res\\config.ini");
  getColourComponent(colour, &backgroundColour[0]);
  colour = GetPrivateProfileInt(L"DISPLAY", L"background_colour_g", 0, L".\\res\\config.ini");
  getColourComponent(colour, &backgroundColour[1]);
  colour = GetPrivateProfileInt(L"DISPLAY", L"background_colour_b", 0, L".\\res\\config.ini");
  getColourComponent(colour, &backgroundColour[2]);
  backgroundColour[3] = 1.0f;
}

float Config::getColourComponent(int colour, float* outDestination)
{
  colour = colour > 255 ? 255 : colour;
  colour = colour < 0 ? 0 : colour;
  float result = colour / 255.0f;
  if(NULL != outDestination)
  {
    *outDestination = result;
  }
  return result;
}
