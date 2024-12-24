/*
  touchNumInput.h - Library for Touch LCD - small number input LCD field
  Created by Jochen Kober, December 2021.
  Released into the public domain.

  VERSION INFO

    V 2.0   First public version



  NUM_PAD_4X3 (0)

  +---+---+---+---+
  | 1 | 2 | 3 | 4 |
  +---+---+---+---+
  | 5 | 6 | 7 | 8 |
  +---+---+---+---+
  | 9 | 0 | , | < |
  +---+---+---+---+


  NUM_PAD_SINGLE_LINE (1)

  +---+---+---+---+---+---+---+---+---+---+---+---+
  | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | , | < |
  +---+---+---+---+---+---+---+---+---+---+---+---+


  NUM_PAD_8X2 (2)

  +---+---+---+---+---+---+---+---+
  | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
  +---+---+---+---+---+---+---+---+
  | 9 | 0 | , | + | - | < |  CLR  |
  +---+---+---+---+---+---+---+---+



  NUM_PAD_BIG (3)

  +---+---+---+-------+
  | 7 | 8 | 9 |  CLR  |
  +---+---+---+-------+
  | 4 | 5 | 6 |  DEL  |
  +---+---+---+-------+
  | 1 | 2 | 3 |       |
  +---+---+---+   OK  +
  |+/-| 0 | , |       |
  +---+---+---+-------+

*/



#ifndef _TOUCH_NUMBER_INPUT_
#define _TOUCH_NUMBER_INPUT_

#include <TFT_eSPI.h>
#include <Free_Fonts.h>

#define PAD_TYPE_COUNT              4


#define NUM_PAD_COMMA               10
#define NUM_PAD_PLUS                11
#define NUM_PAD_MINUS               12
#define NUM_PAD_DEL                 13
#define NUM_PAD_CLEAR               14
#define NUM_PAD_SIGN_TOGGLE         15
#define NUM_PAD_OK                  16
#define NUM_PAD_ALL                 0xFFFF

#define MODE_4X3                    0
#define MODE_SINGLE_LINE            1
#define MODE_8x2                    2
#define MODE_BIG                    3

#define ERROR_NONE                  0
#define ERROR_TFT_NOT_INITIALIZED   1
#define ERROR_INDEX_OVERRUN         2

  
class touchNumInput
{
  public:
    touchNumInput(void);
    uint8_t init(uint16_t x, uint16_t y, uint8_t mode, TFT_eSPI *tft);
    void setFont(const GFXfont *f);
    uint8_t show(void);
    uint8_t enable(bool (*CB_numInputChanged)(uint8_t), void (*CB_outputCallback)(float, bool, bool));
    uint8_t disable(void);
    void enablePad(uint8_t index);
    void disablePad(uint8_t index);
    void setDecimals(uint8_t decimals);

    void isTouched(uint16_t x, uint16_t y);
    void isReleased(void);

    float getValue(void) const;
    char *getValueString(void);
    void setValue(float value);

  private:
    void checkInput(uint8_t key);
    void drawPad(uint8_t index, bool isSelected, bool skipFill);
    uint8_t selectPad(uint8_t index);
    uint8_t unselectPad(uint8_t index);
    void enablePad(uint8_t index, bool internal);
    void disablePad(uint8_t index, bool internal);
    uint8_t correctIndex(uint8_t index);
    void setValueString(void);
};

#endif