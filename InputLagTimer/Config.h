#include <DirectXMath.h>

class Config
{
public:
  static void config();

  static double longestFrameTime;
  static double highestRenderVariance;

  static int numColumns;
  static DirectX::XMVECTOR fontColour;
  static float backgroundColour[4];

protected:
  /**
   * @param outDestination if not NULL, the result will be written to this address.
   */
  static float getColourComponent(int colour, float* outDestination = NULL);
};
