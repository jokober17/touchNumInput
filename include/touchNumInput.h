/*
  touchNumInput.h - Library for Touch LCD - small number input LCD field
  Created by Jochen Kober, December 2021.
  Released into the public domain.
*/



#ifndef _TOUCH_NUMBER_INPUT_
#define _TOUCH_NUMBER_INPUT_

#include <TFT_eSPI.h>



#define NUM_PAD_1                   0
#define NUM_PAD_2                   1
#define NUM_PAD_3                   2
#define NUM_PAD_4                   3
#define NUM_PAD_5                   4
#define NUM_PAD_6                   5
#define NUM_PAD_7                   6
#define NUM_PAD_8                   7
#define NUM_PAD_9                   8
#define NUM_PAD_0                   9
#define NUM_PAD_COMMA               10
#define NUM_PAD_PLUS                11
#define NUM_PAD_MINUS               12
#define NUM_PAD_DEL                 13
#define NUM_PAD_CLEAR               14
#define NUM_PAD_ALL                 0x7FFF

#define MODE_4X3                    0
#define MODE_SINGLE_LINE            1
#define MODE_8x2                    2

#define ERROR_NONE                  0
#define ERROR_TFT_NOT_INITIALIZED   1
#define ERROR_INDEX_OVERRUN         2

  
class touchNumInput
{
  public:
    touchNumInput(void);
    uint8_t init(uint16_t x, uint16_t y, uint8_t mode, TFT_eSPI *tft);
    uint8_t show(void);
    uint8_t enable(void (*btnFunction)(uint8_t));
    uint8_t disable(void);
    void setDisabledPads(uint16_t mask);
    void setEnabledPads(uint16_t mask);
    void enablePad(uint8_t index);
    void disablePad(uint8_t index);

    void isTouched(uint16_t x, uint16_t y);
    void isReleased(void);
    void setLimits(float min, float max);
    void setDecimals(uint8_t decimals);

    float getValue(void) const;
    char *getValueString(void);
    void setValue(float value);



    
  private:
    void checkInput(uint8_t key);
    void drawPad(uint8_t index, bool isSelected);
    uint8_t selectPad(uint8_t index);
    uint8_t unselectPad(uint8_t index);

    uint8_t correctIndex(uint8_t index);
    void setValueString(void);

    const char *_numPad8x2[15] =         { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", ".", "+", "-", "<", "CLR"};
    const uint8_t _referenceTable8x2[15] = {  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14};
    const char *_numPad4x3[15] =         { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", ".", "<", "", "", ""};
    const uint8_t _referenceTable4x3[15] = {  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  13, 13, 13, 13};
    const char *_numPad12x1[15] =         { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", ".", "<", "", "", ""};
    const uint8_t _referenceTable12x1[15] = {  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10, 13,  13, 13, 13};

    uint8_t _mode;


  protected:

    int8_t  selectedPad;
    uint16_t frameColor;   // number pad frame / line color
    uint16_t textColor;    // text color if enabled and not highlighted
    uint16_t backColor;    // back color if not selected
    uint16_t highlightBackColor;
    uint16_t highlightColor;
    uint16_t disabledColor;

    TFT_eSPI *_tft;
    uint16_t _x;
    uint16_t _y;
    uint16_t _enabledPad;
    const GFXfont  *_font;
    bool    _enabled;
    bool    _isVisible;
    int8_t  _lastHighlighted;
    void    (*_btnFunction)(uint8_t);
    float   _minValue;
    float   _maxValue;
    int8_t  _position;
    uint8_t _decimals;
    float   _value;
    float   _signedValue;
    bool    _sign;      // true = poitiv, false = negative
    char    _valueString[15];
};

#endif