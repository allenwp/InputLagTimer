#pragma once
class Model
{
public:
  /**
   * Custom fixed point type
   */
  union ValueType
  {
    UINT32 container;
    struct
    {
    /**
     * Range of 0-999
     */
    UINT16 high;

    /**
     * Range of 0-99
     */
    UINT16 low;
    } value;
  };

  Model(void);
  virtual ~Model(void);

  void update();

  ValueType* getValuePtr();
  unsigned int* getFpsPtr();
  
private:
  void incrementLow(Model::ValueType* value);
  void incrementHigh(Model::ValueType* value);

  ValueType mValue;
  unsigned int mFps;
};

