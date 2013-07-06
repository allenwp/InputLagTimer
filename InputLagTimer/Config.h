#include <DirectXMath.h>

class Config
{
public:
  static void config();

  static double lowestAccuracy;
  static double highestJitter;

  static int numColumns;
  static DirectX::XMVECTOR fontColour;
  static float backgroundColour[4];

protected:
  /**
   * @param outDestination if not NULL, the result will be written to this address.
   */
  static float getColourComponent(int colour, float* outDestination = NULL);
};
