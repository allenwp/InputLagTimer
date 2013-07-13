#include "stdafx.h"
#include "Config.h"

double Config::longestFrameTime = 0;
double Config::highestRenderVariance = 0;

int Config::numColumns = 2;
DirectX::XMVECTOR Config::fontColour = { 0.0, 0.0, 0.0, 1.0f };
float Config::backgroundColour[4] = { 1.0, 1.0, 1.0, 1.0f };

void Config::config()
{
  int longestFrameTime = GetPrivateProfileInt(L"FAILSAFES", L"longest_frame_time", 10, L".\\config.ini");
  Config::longestFrameTime = (double)longestFrameTime / 10000.0;
  int highestRenderVariance = GetPrivateProfileInt(L"FAILSAFES", L"highest_render_variance", 10, L".\\config.ini");
  Config::highestRenderVariance = (double)highestRenderVariance / 10000.0;
  
  Config::numColumns = GetPrivateProfileInt(L"DISPLAY", L"num_columns", 2, L".\\config.ini");

  int colour = GetPrivateProfileInt(L"DISPLAY", L"font_colour_r", 255, L".\\config.ini");
  float r = getColourComponent(colour);
  colour = GetPrivateProfileInt(L"DISPLAY", L"font_colour_g", 255, L".\\config.ini");
  float g = getColourComponent(colour);
  colour = GetPrivateProfileInt(L"DISPLAY", L"font_colour_b", 255, L".\\config.ini");
  float b = getColourComponent(colour);
  DirectX::XMVECTOR newColour = { r, g, b, 1.0f };
  fontColour = newColour;

  colour = GetPrivateProfileInt(L"DISPLAY", L"background_colour_r", 0, L".\\config.ini");
  getColourComponent(colour, &backgroundColour[0]);
  colour = GetPrivateProfileInt(L"DISPLAY", L"background_colour_g", 0, L".\\config.ini");
  getColourComponent(colour, &backgroundColour[1]);
  colour = GetPrivateProfileInt(L"DISPLAY", L"background_colour_b", 0, L".\\config.ini");
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
