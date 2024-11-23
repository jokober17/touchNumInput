
#include <Arduino.h>
#include <touchNumInput.h>



//
//
//
touchNumInput::touchNumInput(void) {

} 

//
//
//
uint8_t touchNumInput::init(uint16_t x, uint16_t y, uint8_t mode, TFT_eSPI *tft) {

  if (tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);
  if (mode != MODE_4X3 && mode != MODE_SINGLE_LINE && mode != MODE_8x2) return (ERROR_INDEX_OVERRUN);

  _tft = tft;
  _x = x;
  _y = y;
  _mode = mode;
  
  _enabledPad = 0x7FFF;   // enable all pads by default
  frameColor = TFT_BLACK;
  textColor = TFT_BLACK;
  backColor = TFT_WHITE;
  highlightColor = TFT_WHITE;
  highlightBackColor = TFT_BLACK;
  disabledColor = TFT_LIGHTGREY;
  _font = FF22;
  _enabled = false;
  _isVisible = false;
  _lastHighlighted = -1;
  _btnFunction = NULL;
  _maxValue = 0;        // no limit if min and max value are the same
  _minValue = 0;
  _position = 1;
  _decimals = 3;
  _value = 0;
  _sign = true;


  selectedPad = -1;

  if (_mode == MODE_8x2) _enabledPad &= ~(1 << NUM_PAD_PLUS);

  return(ERROR_NONE);
}

//
//
//
uint8_t touchNumInput::show(void) {
  uint8_t loop;
  uint16_t x, y;

  if (_tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);
  
  if (_mode == MODE_4X3) {
    _tft->fillRect(_x, _y, 160, 160, backColor);
    _tft->drawFastHLine(_x, _y, 160, frameColor);
    _tft->drawFastHLine(_x, _y+40, 160, frameColor);
    _tft->drawFastHLine(_x, _y+80, 160, frameColor);
    _tft->drawFastHLine(_x, _y+120, 160, frameColor);
    _tft->drawFastVLine(_x, _y, 120, frameColor);
    _tft->drawFastVLine(_x+40, _y, 120, frameColor);
    _tft->drawFastVLine(_x+80, _y, 120, frameColor);
    _tft->drawFastVLine(_x+120, _y, 120, frameColor);
    _tft->drawFastVLine(_x+160, _y, 120, frameColor);
    // set and draw text
    _tft->setFreeFont(FF22);
    _tft->setTextDatum(MC_DATUM);
    x = _x + 20;
    y = _y + 20;
    for (loop=0; loop<12; loop++) {
      if ((1<<loop) & _enabledPad) _tft->setTextColor(textColor);
      else
        _tft->setTextColor(disabledColor);
      
      _tft->drawString(_numPad4x3[loop], x, y);
      if (loop%4 == 3) {
        x = _x + 20;
        y += 40;
      }
      else {
        x += 40;
      }
    }
  }

  if (_mode == MODE_SINGLE_LINE) {
    _tft->fillRect(_x, _y, 39 * 12, 40, backColor);
    _tft->drawFastHLine(_x, _y, 39 * 12, frameColor);
    _tft->drawFastHLine(_x, _y + 39, 39 * 12, frameColor);
    for (loop=0; loop<13; loop++) {
      _tft->drawFastVLine(_x + loop * 39, _y, 39, frameColor);
    }

    // set and draw text
    _tft->setFreeFont(FF22);
    _tft->setTextDatum(MC_DATUM);
    x = _x + 19;
    y = _y + 19;
    for (loop=0; loop<12; loop++) {
      if ((1<<loop) & _enabledPad) _tft->setTextColor(textColor);
      else
        _tft->setTextColor(disabledColor);
      
      _tft->drawString(_numPad12x1[loop], x, y);
      x = x + 39;
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

    // set and draw text
    _tft->setFreeFont(FF22);
    _tft->setTextDatum(MC_DATUM);
    x = _x + 20;
    y = _y + 20;
    for (loop=0; loop<15; loop++) {
      if ((1<<loop) & _enabledPad) _tft->setTextColor(textColor);
      else
        _tft->setTextColor(disabledColor);
      
      if (loop == 14) {
        _tft->drawString(_numPad8x2[loop], x + 20, y);
      }
      else {
        _tft->drawString(_numPad8x2[loop], x, y);
        if (loop%8 == 7) {
          x = _x + 20;
          y += 40;
        }
        else {
          x += 40;
        }
      }
    }
  }

  _isVisible = true;
  return(ERROR_NONE);
}

//
//
//
void touchNumInput::drawPad(uint8_t index, bool isSelected) {

  if (_mode == MODE_4X3) _tft->fillRect(_x + 1 + ((index % 4) * 40), _y + 1 + ((index / 4) * 40), 39, 39, (isSelected)? highlightBackColor : backColor);
  if (_mode == MODE_SINGLE_LINE) _tft->fillRect(_x + 1 + index * 39, _y + 1, 38, 38, (isSelected)? highlightBackColor : backColor);
  if (_mode == MODE_8x2) {
    if (index == 14) { _tft->fillRect(_x + 241, _y + 41, 79, 39, (isSelected)? highlightBackColor : backColor); }
    else { _tft->fillRect(_x + 1 + ((index % 8) * 40), _y + 1 + ((index / 8) * 40), 39, 39, (isSelected)? highlightBackColor : backColor); }
  }

  _tft->setFreeFont(_font);
  if (!(_enabledPad & (1 << index))) _tft->setTextColor(disabledColor);
  else
    _tft->setTextColor((isSelected)? highlightColor : textColor);
    
  _tft->setTextDatum(MC_DATUM);

  if (_mode == MODE_4X3) _tft->drawString(_numPad4x3[index], _x + 20 + ((index % 4) * 40), _y + 20 + ((index / 4) * 40));
  if (_mode == MODE_SINGLE_LINE) _tft->drawString(_numPad12x1[index], _x + 19 + index * 39, _y + 19);
  if (_mode == MODE_8x2) {
    if (index == 14) { _tft->drawString(_numPad8x2[index], _x + 280, _y + 60); }
    else { _tft->drawString(_numPad8x2[index], _x + 20 + ((index % 8) * 40), _y + 20 + ((index / 8) * 40)); }
  }
}

//
//
//
uint8_t touchNumInput::selectPad(uint8_t index){
  if (index > 14) return(ERROR_INDEX_OVERRUN);
  if (_tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);

  drawPad(index, true);
  return(ERROR_NONE);
}


//
//
//
uint8_t touchNumInput::unselectPad(uint8_t index) {
  if (index > 14) return(ERROR_INDEX_OVERRUN);
  if (_tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);

  drawPad(index, false);
  return(ERROR_NONE);
}

//
//
//
uint8_t touchNumInput::enable(void (*btnFunction)(uint8_t) = NULL) {
  if (_tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);
  _enabled = true;
  _btnFunction = btnFunction;
  return(ERROR_NONE);
}

//
//
//
uint8_t touchNumInput::disable(void) {
  if (_tft == NULL) return(ERROR_TFT_NOT_INITIALIZED);
  _enabled = false;
  _btnFunction = NULL;
  return(ERROR_NONE);
}

//
//
//
void touchNumInput::setDisabledPads(uint16_t mask) {
  _enabledPad &= ~(mask & 0x7FFF);
  // refresh number input pad if necessary
  if (_isVisible) show();
}

//
//
//
void touchNumInput::setEnabledPads(uint16_t mask) {
  _enabledPad |= (mask & 0x7FFF);
  // refresh number input pad if necessary
  if (_isVisible) show();
}

//
//
//
void touchNumInput::enablePad(uint8_t index) {
  if (index > 14) return;
  _enabledPad |= (1 << index);
  drawPad(index, false);
}

//
//
//
void touchNumInput::disablePad(uint8_t index) {
  if (index > 14) return;
  _enabledPad &= ~(1 << index);
  drawPad(index, false);
}

//
//
//
void touchNumInput::isTouched(uint16_t x, uint16_t y) {
  if (!_enabled) return;

  // check if number is pressed
  for (uint8_t loop=0; loop<15; loop++) {
    if (_mode == MODE_4X3) {
      if ( ((1<<loop) & _enabledPad) && x >= ((loop % 4) * 40) + _x && x <= (((loop % 4) + 1) * 40) + _x && y >= ((loop / 4) * 40) + _y && y <= (((loop / 4) + 1) * 40) + _y) {
        if (loop != _lastHighlighted) {
          unselectPad(_lastHighlighted);
          selectPad(loop);
          _lastHighlighted = loop;
        }
        loop = 15;
      }
    }

    if (_mode == MODE_SINGLE_LINE) {
      if ( ((1<<loop) & _enabledPad) && x >= (loop * 39 + _x) && x <= ((loop + 1) * 40 + _x) && y >= _y && y <= _y + 39) {
        if (loop != _lastHighlighted) {
          unselectPad(_lastHighlighted);
          selectPad(loop);
          _lastHighlighted = loop;
        }
        loop = 15;
      }
    }

    if (_mode == MODE_8x2) {
      if ( ((1<<loop) & _enabledPad) && 
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
  }
}

//
//
//
void touchNumInput::isReleased(void) {
  if (_enabled && _lastHighlighted != -1) {
    unselectPad(_lastHighlighted);
    selectedPad = _lastHighlighted;
    _lastHighlighted = -1;

    // call function if defined
    checkInput(selectedPad);
    if (_btnFunction != NULL) _btnFunction(selectedPad);
  }
}

//
//
//
float touchNumInput::getValue(void) const {
  return(_signedValue);
}

//
//
//
char *touchNumInput::getValueString(void) {
  return(_valueString);
}

//
//
//
void touchNumInput::setValue(float value) {
  // set to limits if enabled
  if (_minValue != _maxValue) {
    if (value < _minValue) value = _minValue;
    if (value > _maxValue) value = _maxValue;
  }

  if (value == 0.0) {
    _sign = true;
    _position = 1;
  }
  else {
    // preset variables
    if (value < 0) {
      _sign = false;
      _value = -value;
    }
    else {
      _sign = true;
      _value = value;
    }    

    // preset _position
    if (_decimals == 0) {
      _position = 1;
      disablePad(NUM_PAD_COMMA);
    }
    else {
      _position = -_decimals;
      disablePad(NUM_PAD_COMMA);
    }
  }

  // Serial.printf("Dec: %d, Pos: %d, Value: %0.2f\n", _decimals, _position, value);
  _signedValue = value;
  setValueString();
}

//
//
//
uint8_t touchNumInput::correctIndex(uint8_t index) {
  if (_mode == MODE_4X3) return (_referenceTable4x3[index]);
  if (_mode == MODE_SINGLE_LINE) return (_referenceTable12x1[index]);
  if (_mode == MODE_8x2) return (_referenceTable8x2[index]);
  return (NUM_PAD_CLEAR);
}

//
//
//
void touchNumInput::setValueString(void) {
  switch(_position) {
    case 0:
      sprintf(_valueString, "%.0f.", _signedValue);
      break;
    case 1 ... 9:
      sprintf(_valueString, "%.0f", _signedValue);
      break;
    case -1:
      sprintf(_valueString, "%.1f", _signedValue);
      break;
    case -2:
      sprintf(_valueString, "%.2f", _signedValue);
      break;
    case -3:
      sprintf(_valueString, "%.3f", _signedValue);
      break;
    case -4:
      sprintf(_valueString, "%.4f", _signedValue);
      break;
    case -5:
      sprintf(_valueString, "%.5f", _signedValue);
      break;
  }
  // Serial.printf("Valuestring: %s\n", _valueString);
}

//
//
//
void touchNumInput::checkInput(uint8_t index) {

  float val;

  // save actual value
  val = _value;
  // get correct index
  index = correctIndex(index);

  // work with input
  switch(index) {
    case 0 ... 9:
      if (_position > 0) {
        if (index == NUM_PAD_0) _value *= 10;
        else
          _value = (_value * 10) + index + 1;
        _position = 1;
      }
      else {
        if (_position != -_decimals) {
          _position--;
          if (index != NUM_PAD_0) _value = _value + ((index + 1) * pow10(_position));
        }
      }
      break;
    
    case NUM_PAD_COMMA:
      if (_decimals > 0 && _position != 0) {
        _position = 0;
        disablePad(NUM_PAD_COMMA);
      }
      break;

    case NUM_PAD_PLUS:
      if (!_sign) {
        _sign = true;
        if (_value > _maxValue) _value = _maxValue;
        enablePad(NUM_PAD_MINUS);
        disablePad(NUM_PAD_PLUS);
      }
      break;

    case NUM_PAD_MINUS:
      if (_sign) {
        _sign = false;
        if (-_value < _minValue) _value = _minValue;
        enablePad(NUM_PAD_PLUS);
        disablePad(NUM_PAD_MINUS);
      }
      break;

    case NUM_PAD_DEL:
      if (_position == 0) {
        _position = 1;
        if (_minValue < 0) enablePad(NUM_PAD_COMMA);
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

        if (_position > 0 && _value > 0) {
          _value = truncf(_value /= 10);
        }
      }
      if (_value == 0) {
        _sign = true;
        if (_minValue < 0) enablePad(NUM_PAD_MINUS);
        disablePad(NUM_PAD_PLUS);
      }
      break;

    case NUM_PAD_CLEAR:
      if (_minValue > 0) _value = _minValue;
      else
        _value = 0;
      _position = 1;
      _sign = true;
      disablePad(NUM_PAD_PLUS);
      if (_minValue < 0) enablePad(NUM_PAD_MINUS);
      if (_decimals > 0) enablePad(NUM_PAD_COMMA);
      break;
  }

  // check limits and add value sign
  if (!_sign) {
    // negativ value
    if (_minValue != _maxValue) {
      if (-_value < _minValue) _value = val;
      else if (-_value > _maxValue) _value = val;
    }
    val = -_value;
  } 
  else {
    // positiv value
    if (_minValue != _maxValue) {
      if (_value < _minValue) _value = val;
      else if (_value > _maxValue) _value = val;
    }
    val = _value;
  }

  // print value to string
  _signedValue = val;
  setValueString();
}

//
//
//
void touchNumInput::setLimits(float minVal, float maxVal) {
  _minValue = min(minVal, maxVal);
  _maxValue = max(minVal, maxVal);

  if (_minValue >= 0) disablePad(NUM_PAD_MINUS);
}

//
//
//
void touchNumInput::setDecimals(uint8_t decimals) {
  _decimals = decimals;
  if (_decimals > 3) _decimals = 3;
  if (_decimals == 0) disablePad(NUM_PAD_COMMA);
}