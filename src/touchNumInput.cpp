
#include <Arduino.h>
#include <touchNumInput.h>



/***************************************************************************************
** Function name:           touchNumInput
** Description:             constructor
***************************************************************************************/
touchNumInput::touchNumInput(void) {
  init(0, 0, MODE_4X3, NULL);
} 

/***************************************************************************************
** Function name:           init
** Description:             init function to initialize class
***************************************************************************************/
uint8_t touchNumInput::init(uint16_t x, uint16_t y, uint8_t mode, TFT_eSPI *tft) {

  if (tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);
  if (mode >= PAD_TYPE_COUNT) return (ERROR_INDEX_OVERRUN);

  _tft = tft;
  _x = x;
  _y = y;
  _mode = mode;
  
  _enabledPad = 0xFFFF;   // enable all pads by default
  _userDisabledPad = 0x0000;    // all enabled by user
  frameColor = TFT_BLACK;
  textColor = TFT_BLACK;
  backColor = TFT_WHITE;
  highlightColor = TFT_WHITE;
  highlightBackColor = TFT_BLACK;
  disabledColor = TFT_LIGHTGREY;
  _font = &FreeSansBold12pt7b;
  _enabled = false;
  _isVisible = false;
  _lastHighlighted = -1;
  _numInputChangedCallback = NULL;
  _outputCallback = NULL;
  _position = 1;
  _decimals = 3;
  _value = 0;
  selectedPad = -1;

  if (_mode == MODE_8x2) _userDisabledPad = (1 << NUM_PAD_PLUS | 1 << NUM_PAD_MINUS);
  return(ERROR_NONE);
}

/***************************************************************************************
** Function name:           setFont
** Description:             set font to use for text - not used in MODE_BIG
***************************************************************************************/
void touchNumInput::setFont(const GFXfont *f) {
  _tft->setFreeFont(f);
}

/***************************************************************************************
** Function name:           drawPad
** Description:             draw single pad highlighted or not
***************************************************************************************/
void touchNumInput::drawPad(uint8_t index, bool isSelected, bool skipFill = false) {

  if (!skipFill) {
    // fill rectangle only if skipFill = false
    if (_mode == MODE_4X3) _tft->fillRect(_x + 1 + ((index % 4) * 40), _y + 1 + ((index / 4) * 40), 39, 39, (isSelected)? highlightBackColor : backColor);
    if (_mode == MODE_SINGLE_LINE) _tft->fillRect(_x + 1 + index * 39, _y + 1, 38, 38, (isSelected)? highlightBackColor : backColor);
    if (_mode == MODE_8x2) {
      if (index == 14) { _tft->fillRect(_x + 241, _y + 41, 79, 39, (isSelected)? highlightBackColor : backColor); }
      else { _tft->fillRect(_x + 1 + ((index % 8) * 40), _y + 1 + ((index / 8) * 40), 39, 39, (isSelected)? highlightBackColor : backColor); }
    }
    if (_mode == MODE_BIG) _tft->fillRect(_x + 2 + (index % 4) * 50, _y + 2 + (index / 4) * 50, 47 + (((index % 4) == 3)? 20 : 0), 47 + ((index == 11)? 50 : 0), (isSelected)? highlightBackColor : backColor);
  }

  // setup text font and text color 
  _tft->setFreeFont(_font);
  if (!(_enabledPad & ~_userDisabledPad & (1 << index))) _tft->setTextColor(disabledColor);
  else
    _tft->setTextColor((isSelected)? highlightColor : textColor);
    
  _tft->setTextDatum(MC_DATUM);

  if (_mode == MODE_4X3) _tft->drawString(_numPadStr[_mode][index], _x + 20 + ((index % 4) * 40), _y + 20 + ((index / 4) * 40));
  if (_mode == MODE_SINGLE_LINE) _tft->drawString(_numPadStr[_mode][index], _x + 19 + index * 39, _y + 19);
  if (_mode == MODE_8x2) {
    if (index == 14) { _tft->drawString(_numPadStr[_mode][index], _x + 280, _y + 60); }
    else { _tft->drawString(_numPadStr[_mode][index], _x + 20 + ((index % 8) * 40), _y + 20 + ((index / 8) * 40)); }
  }
  if (_mode == MODE_BIG) {
    _tft->setFreeFont((((index % 4) == 3)? &FreeSansBold9pt7b : &FreeSansBold12pt7b));    
    _tft->drawString(_numPadStr[_mode][index], _x + (index % 4) * 50 + (((index % 4) == 3)? 35 : 25), _y + (index / 4) * 50 + (((index == 11)? 50 : 25)));
  }
}

/***************************************************************************************
** Function name:           show
** Description:             initially draw number input pad depending on mode selected
***************************************************************************************/
uint8_t touchNumInput::show(void) {
  uint8_t loop;
  uint16_t x, y;

  if (_tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);
  
  if (_mode == MODE_BIG) {
    _tft->fillRect(_x, _y, 200, 220, backColor);
    _tft->drawFastHLine(_x, _y, 220, frameColor);
    _tft->drawFastHLine(_x, _y+50, 220, frameColor);
    _tft->drawFastHLine(_x, _y+100, 220, frameColor);
    _tft->drawFastHLine(_x, _y+150, 150, frameColor);
    _tft->drawFastHLine(_x, _y+200, 220, frameColor);
    _tft->drawFastVLine(_x, _y, 200, frameColor);
    _tft->drawFastVLine(_x+50, _y, 200, frameColor);
    _tft->drawFastVLine(_x+100, _y, 200, frameColor);
    _tft->drawFastVLine(_x+150, _y, 200, frameColor);
    _tft->drawFastVLine(_x+220, _y, 200, frameColor);
  }

  if (_mode == MODE_4X3) {
    _tft->fillRect(_x, _y, 160, 120, backColor);
    _tft->drawFastHLine(_x, _y, 160, frameColor);
    _tft->drawFastHLine(_x, _y+40, 160, frameColor);
    _tft->drawFastHLine(_x, _y+80, 160, frameColor);
    _tft->drawFastHLine(_x, _y+120, 160, frameColor);
    _tft->drawFastVLine(_x, _y, 120, frameColor);
    _tft->drawFastVLine(_x+40, _y, 120, frameColor);
    _tft->drawFastVLine(_x+80, _y, 120, frameColor);
    _tft->drawFastVLine(_x+120, _y, 120, frameColor);
    _tft->drawFastVLine(_x+160, _y, 120, frameColor);
  }

  if (_mode == MODE_SINGLE_LINE) {
    _tft->fillRect(_x, _y, 39 * 12, 40, backColor);
    _tft->drawFastHLine(_x, _y, 39 * 12, frameColor);
    _tft->drawFastHLine(_x, _y + 39, 39 * 12, frameColor);
    for (loop=0; loop<13; loop++) {
      _tft->drawFastVLine(_x + loop * 39, _y, 39, frameColor);
    }
  }

  if (_mode == MODE_8x2) {
    _tft->fillRect(_x, _y, 320, 80, backColor);
    _tft->drawFastHLine(_x, _y, 320, frameColor);
    _tft->drawFastHLine(_x, _y+40, 320, frameColor);
    _tft->drawFastHLine(_x, _y+80, 320, frameColor);
    _tft->drawFastVLine(_x, _y, 80, frameColor);
    _tft->drawFastVLine(_x+40, _y, 80, frameColor);
    _tft->drawFastVLine(_x+80, _y, 80, frameColor);
    _tft->drawFastVLine(_x+120, _y, 80, frameColor);
    _tft->drawFastVLine(_x+160, _y, 80, frameColor);
    _tft->drawFastVLine(_x+200, _y, 80, frameColor);
    _tft->drawFastVLine(_x+240, _y, 80, frameColor);
    _tft->drawFastVLine(_x+280, _y, 40, frameColor);
    _tft->drawFastVLine(_x+320, _y, 80, frameColor);
  }

  // draw keys
  _tft->setFreeFont(&FreeSansBold12pt7b);
  _tft->setTextDatum(MC_DATUM);

  for (loop=0; loop<_numPadCount[_mode]; loop++) {
    drawPad(loop, false, true);
  }

  _isVisible = true;
  return(ERROR_NONE);
}

/***************************************************************************************
** Function name:           setDecimals
** Description:             set number of decimmals to use (maximum is 5)
***************************************************************************************/
void touchNumInput::setDecimals(uint8_t decimals) {
  _decimals = decimals;
  if (_decimals > 5) _decimals = 5;
  if (_decimals == 0) disablePad(NUM_PAD_COMMA);
}

/***************************************************************************************
** Function name:           selectPad
** Description:             highlight pad with index
***************************************************************************************/
uint8_t touchNumInput::selectPad(uint8_t index){
  if (index > 14) return(ERROR_INDEX_OVERRUN);
  if (_tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);
  // highlight button
  drawPad(index, true);
  return(ERROR_NONE);
}


/***************************************************************************************
** Function name:           unselectPad
** Description:             draw pad normal with index
***************************************************************************************/
uint8_t touchNumInput::unselectPad(uint8_t index) {
  if (index > 14) return(ERROR_INDEX_OVERRUN);
  if (_tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);
  // show button normal
  drawPad(index, false);
  return(ERROR_NONE);
}

/***************************************************************************************
** Function name:           enable
** Description:             enable number input field
***************************************************************************************/
uint8_t touchNumInput::enable(bool (*CB_numInputChanged)(float newValue) = NULL, void (*CB_outputCallback)(float value, bool okPressed, bool showComma) = NULL) {
  if (_tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);
  _enabled = true;
  _numInputChangedCallback = CB_numInputChanged;
  _outputCallback = CB_outputCallback;
  return(ERROR_NONE);
}

/***************************************************************************************
** Function name:           disable
** Description:             disable number input field
***************************************************************************************/
uint8_t touchNumInput::disable(void) {
  if (_tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);
  _enabled = false;
  _numInputChangedCallback = NULL;
  return(ERROR_NONE);
}


/***************************************************************************************
** Function name:           enablePad
** Description:             enable single pad dentified by index (NUM_PAD_xxxx)
***************************************************************************************/
void touchNumInput::enablePad(uint8_t index) {
  enablePad(index, false);
}

/***************************************************************************************
** Function name:           enablePad
** Description:             internal routine
***************************************************************************************/
void touchNumInput::enablePad(uint8_t index, bool internal) {
  if (index > 15) return;

  // find index position in reference table
  for (uint8_t i=0; i<15; i++) {
    if (_numRefTable[_mode][i] == index) {
      if (internal) _enabledPad |= (1 << i);
      else
        _userDisabledPad &= ~(1 << i);
      drawPad(i, false);
      return;
    }
  }
}


/***************************************************************************************
** Function name:           disablePad
** Description:             disable single pad dentified by index (NUM_PAD_xxxx)
***************************************************************************************/
void touchNumInput::disablePad(uint8_t index) {
  disablePad(index, false);
}

/***************************************************************************************
** Function name:           disablePad
** Description:             internal routine
***************************************************************************************/
void touchNumInput::disablePad(uint8_t index, bool internal) {
  if (index > 15) return;

  // find index position in reference table
  for (uint8_t i=0; i<15; i++) {
    if (_numRefTable[_mode][i] == index) {
      if (internal) _enabledPad &= ~(1 << i);
      else
        _userDisabledPad |= (1 << i);
      drawPad(i, false);
      return;
    }
  }
}

/***************************************************************************************
** Function name:           isTouched
** Description:             check if key is touched and highlight it
**                          clear previous highlighted button before if necessary
***************************************************************************************/
void touchNumInput::isTouched(uint16_t x, uint16_t y) {
  if (!_enabled) return;

  // check if out of bound
  if (x < _x || x > _x + _numPadSize[_mode][0] || y < _y || y > _y + _numPadSize[_mode][1]) {
    // out of bound
    unselectPad(_lastHighlighted);
    _lastHighlighted = -1;
    return;
  }

  // check if number is pressed
  for (uint8_t loop=0; loop<_numPadCount[_mode]; loop++) {
    if (_mode == MODE_4X3) {
      if ( ((1<<loop) & _enabledPad & ~_userDisabledPad) && x >= ((loop % 4) * 40) + _x && x <= (((loop % 4) + 1) * 40) + _x && y >= ((loop / 4) * 40) + _y && y <= (((loop / 4) + 1) * 40) + _y) {
        if (loop != _lastHighlighted) {
          unselectPad(_lastHighlighted);
          selectPad(loop);
          _lastHighlighted = loop;
        }
        loop = 15;
      }
    }

    if (_mode == MODE_SINGLE_LINE) {
      if ( ((1<<loop) & _enabledPad & ~_userDisabledPad) && x >= (loop * 39 + _x) && x <= ((loop + 1) * 40 + _x) && y >= _y && y <= _y + 39) {
        if (loop != _lastHighlighted) {
          unselectPad(_lastHighlighted);
          selectPad(loop);
          _lastHighlighted = loop;
        }
        loop = 15;
      }
    }

    if (_mode == MODE_8x2) {
      if ( ((1<<loop) & _enabledPad & ~_userDisabledPad) && 
        ((loop == 14 && x >= 241 + _x && x <= 319 + _x && y >= _y + 41 && y <= _y + 79) ||
        (loop < 14 && x >= ((loop % 8) * 40) + _x && x <= (((loop % 8) + 1) * 40) + _x && y >= ((loop / 8) * 40) + _y && y <= (((loop / 8) + 1) * 40) + _y) )) {
          if (loop != _lastHighlighted) {
            unselectPad(_lastHighlighted);
            selectPad(loop);
            _lastHighlighted = loop;
          }
        loop = 15;
      }
    }

    if (_mode == MODE_BIG) {
      // check if outside boundry
      if (x <= _x || x >= _x+220 || y <= _y || y >= _y+200) {
          unselectPad(_lastHighlighted);
          _lastHighlighted = -1;
          return;
      }
      if ((1<<loop) & _enabledPad & ~_userDisabledPad) {
        if (x > _x + loop%4 * 50 && x < _x + (loop%4 * 50)+ ((loop%4==3)? 70 : 50) && y > _y + loop/4 * 50  && y < _y + loop/4 * 50 + ((loop==11)? 100 : 50)) {
          if (loop != _lastHighlighted) {
            unselectPad(_lastHighlighted);
            selectPad(loop);
            _lastHighlighted = loop;
          }
        loop = 15;
        }
      }      
    }
  }
}

/***************************************************************************************
** Function name:           isReleased
** Description:             called if touched key has been released
***************************************************************************************/
void touchNumInput::isReleased(void) {
  bool ret = true;
  float prevValue;

  if (_enabled && _lastHighlighted != -1) {
    unselectPad(_lastHighlighted);
    selectedPad = _lastHighlighted;
    _lastHighlighted = -1;

    // save actual value
    prevValue = _value;
    checkInput(selectedPad);
    if (_numInputChangedCallback != NULL) ret = _numInputChangedCallback(_value);
    if (ret == true) {
      if (_outputCallback != NULL)  _outputCallback(_value, _okPressed, _position>0? false:true);
      // reset ok pressed flag
      _okPressed = false;
    }
    else {
      _value = prevValue;
      _okPressed = false;
    }
  }
}

/***************************************************************************************
** Function name:           getValue
** Description:             get actual float value
***************************************************************************************/
float touchNumInput::getValue(void) const {
  return(_value);
}

/***************************************************************************************
** Function name:           getValueString
** Description:             get actual float value as string
***************************************************************************************/
char *touchNumInput::getValueString(void) {
  // calc new value string & return
  switch(_position) {
    case 0:
      sprintf(_valueString, "%.0f,", _value);
      break;
    case 1 ... 9:
      sprintf(_valueString, "%.0f", _value);
      break;
    case -1:
      sprintf(_valueString, "%.1f", _value);
      break;
    case -2:
      sprintf(_valueString, "%.2f", _value);
      break;
    case -3:
      sprintf(_valueString, "%.3f", _value);
      break;
    case -4:
      sprintf(_valueString, "%.4f", _value);
      break;
    case -5:
      sprintf(_valueString, "%.5f", _value);
      break;
  }
  return(_valueString);
}

/***************************************************************************************
** Function name:           setValue
** Description:             set actual value
***************************************************************************************/
void touchNumInput::setValue(float value) {
  _value = value;

  if (value == 0.0) {
    _position = 1;
  }
  else {
    // preset _position
    if (_decimals == 0) {
      _position = 1;
      disablePad(NUM_PAD_COMMA, true);
    }
    else {
      _position = -_decimals;
      disablePad(NUM_PAD_COMMA, true);
    }
  }
}

/***************************************************************************************
** Function name:           correctIndex
** Description:             correct key index for different key pads
***************************************************************************************/
uint8_t touchNumInput::correctIndex(uint8_t index) {
  return (_numRefTable[_mode][index]);
}

/***************************************************************************************
** Function name:           checkInput
** Description:             cheak input key and work with it
***************************************************************************************/
void touchNumInput::checkInput(uint8_t index) {
  // get correct index
  index = correctIndex(index);

  // work with input
  switch(index) {
    case 0 ... 9:
      if (_position > 0) {
        if (index == 0) _value *= 10;
        else {
          if (_value >= 0)   _value = (_value * 10) + index;
          else
            _value = (_value * 10) - index;
        }
          
        _position = 1;
      }
      else {
        if (_position != -_decimals) {
          _position--;
          if (index != 0) {
            if (_value >= 0) _value = _value + (index * pow10(_position));
            else
              _value = _value - (index * pow10(_position));
          }
        }
      }
      break;
    
    case NUM_PAD_COMMA:
      if (_decimals > 0 && _position != 0) {
        _position = 0;
        disablePad(NUM_PAD_COMMA, true);
      }
      break;

    case NUM_PAD_PLUS:
    case NUM_PAD_MINUS:
      _value = -_value;
      break;

    case NUM_PAD_DEL:
      if (_position == 0) {
        _position = 1;
        if (_decimals > 0) enablePad(NUM_PAD_COMMA, true);
      }
      else {
        if (_position < 0) {
          _position++;
          switch(_position) {
            case -5: _value = truncf(_value * 100000.0) / 100000.0; break;
            case -4: _value = truncf(_value * 10000.0) / 10000.0; break;
            case -3: _value = truncf(_value * 1000.0) / 1000.0; break;
            case -2: _value = truncf(_value * 100.0) / 100.0; break;
            case -1: _value = truncf(_value * 10.0) / 10.0; break;
            case 0:
              _value = truncf(_value);
              if (_decimals == 0) _position++;
              break;
          }
        }

        if (_position > 0 && _value != 0) {
          _value = truncf(_value /= 10);
        }
      }
      break;

    case NUM_PAD_SIGN_TOGGLE:
      _value = -_value;
      break;

    case NUM_PAD_CLEAR:
      _value = 0;
      _position = 1;
      if (_decimals > 0) enablePad(NUM_PAD_COMMA, true);
      break;

    case NUM_PAD_OK:
      _okPressed = true;
      return;
  }

  // enable/disable +/- in mode 8x2
  if (_mode == MODE_8x2) {
    if (_value < 0) {
      disablePad(NUM_PAD_MINUS, true);
      enablePad(NUM_PAD_PLUS, true);
    }
    else if (_value > 0) {
      enablePad(NUM_PAD_MINUS, true);
      disablePad(NUM_PAD_PLUS, true);
    }
    else {
      disablePad(NUM_PAD_MINUS, true);
      disablePad(NUM_PAD_PLUS, true);
    }
  }
}

