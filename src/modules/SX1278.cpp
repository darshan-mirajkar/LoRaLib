#include "SX1278.h"

SX1278::SX1278(Module* mod) : SX127x(mod) {
  
}

int16_t SX1278::begin(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, int8_t power, uint8_t currentLimit, uint16_t preambleLength, uint8_t gain) {
  // execute common part
  int16_t state = SX127x::begin(SX1278_CHIP_VERSION, syncWord, currentLimit, preambleLength);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // configure settings not accessible by API
  state = config();
  if(state != ERR_NONE) {
    return(state);
  }
  
  // configure publicly accessible settings
  state = setFrequency(freq);
  state |= setBandwidth(bw);
  state |= setSpreadingFactor(sf);
  state |= setCodingRate(cr);
  state |= setOutputPower(power);
  state |= setGain(gain);
  return(state);
}

int16_t SX1278::setFrequency(float freq) {
  // check frequency range
  if((freq < 137.0) || (freq > 525.0)) {
    return(ERR_INVALID_FREQUENCY);
  }
  
  // sensitivity optimization for 500kHz bandwidth
  // see SX1276/77/78 Errata, section 2.1 for details
  if(_bw == 500.0) {
    if((freq >= 862.0) && (freq <= 1020.0)) {
      _mod->SPIwriteRegister(0x36, 0x02);
      _mod->SPIwriteRegister(0x3a, 0x64);
    } else if((freq >= 410.0) && (freq <= 525.0)) {
      _mod->SPIwriteRegister(0x36, 0x03);
      _mod->SPIwriteRegister(0x3a, 0x65);
    }
  }
  
  // mitigation of receiver spurious response
  // see SX1276/77/78 Errata, section 2.3 for details
  if(_bw == 7.8) {
    _mod->SPIsetRegValue(0x31, 0b0000000, 7, 7);
    _mod->SPIsetRegValue(0x2F, 0x48);
    _mod->SPIsetRegValue(0x30, 0x00);
    freq += 7.8;
  } else if(_bw == 10.4) {
    _mod->SPIsetRegValue(0x31, 0b0000000, 7, 7);
    _mod->SPIsetRegValue(0x2F, 0x44);
    _mod->SPIsetRegValue(0x30, 0x00);
    freq += 10.4;
  } else if(_bw == 15.6) {
    _mod->SPIsetRegValue(0x31, 0b0000000, 7, 7);
    _mod->SPIsetRegValue(0x2F, 0x44);
    _mod->SPIsetRegValue(0x30, 0x00);
    freq += 15.6;
  } else if(_bw == 20.8) {
    _mod->SPIsetRegValue(0x31, 0b0000000, 7, 7);
    _mod->SPIsetRegValue(0x2F, 0x44);
    _mod->SPIsetRegValue(0x30, 0x00);
    freq += 20.8;
  } else if(_bw == 31.25) {
    _mod->SPIsetRegValue(0x31, 0b0000000, 7, 7);
    _mod->SPIsetRegValue(0x2F, 0x44);
    _mod->SPIsetRegValue(0x30, 0x00);
    freq += 31.25;
  } else if(_bw == 41.7) {
    _mod->SPIsetRegValue(0x31, 0b0000000, 7, 7);
    _mod->SPIsetRegValue(0x2F, 0x44);
    _mod->SPIsetRegValue(0x30, 0x00);
    freq += 41.7;
  } else if(_bw == 62.5) {
    _mod->SPIsetRegValue(0x31, 0b0000000, 7, 7);
    _mod->SPIsetRegValue(0x2F, 0x40);
    _mod->SPIsetRegValue(0x30, 0x00);
  } else if(_bw == 125.0) {
    _mod->SPIsetRegValue(0x31, 0b0000000, 7, 7);
    _mod->SPIsetRegValue(0x2F, 0x40);
    _mod->SPIsetRegValue(0x30, 0x00);
  } else if(_bw == 250.0) {
    _mod->SPIsetRegValue(0x31, 0b0000000, 7, 7);
    _mod->SPIsetRegValue(0x2F, 0x40);
    _mod->SPIsetRegValue(0x30, 0x00);
  } else if(_bw == 500.0) {
    _mod->SPIsetRegValue(0x31, 0b1000000, 7, 7);
  }
  
  // set frequency
  return(SX127x::setFrequencyRaw(freq));
}

int16_t SX1278::setBandwidth(float bw) {
  uint8_t newBandwidth;
  
  // check alowed bandwidth values
  if(bw == 7.8) {
    newBandwidth = SX1278_BW_7_80_KHZ;
  } else if(bw == 10.4) {
    newBandwidth = SX1278_BW_10_40_KHZ;
  } else if(bw == 15.6) {
    newBandwidth = SX1278_BW_15_60_KHZ;
  } else if(bw == 20.8) {
    newBandwidth = SX1278_BW_20_80_KHZ;
  } else if(bw == 31.25) {
    newBandwidth = SX1278_BW_31_25_KHZ;
  } else if(bw == 41.7) {
    newBandwidth = SX1278_BW_41_70_KHZ;
  } else if(bw == 62.5) {
    newBandwidth = SX1278_BW_62_50_KHZ;
  } else if(bw == 125.0) {
    newBandwidth = SX1278_BW_125_00_KHZ;
  } else if(bw == 250.0) {
    newBandwidth = SX1278_BW_250_00_KHZ;
  } else if(bw == 500.0) {
    newBandwidth = SX1278_BW_500_00_KHZ;
  } else {
    return(ERR_INVALID_BANDWIDTH);
  }
  
  // set bandwidth and if successful, save the new setting
  int16_t state = SX1278::setBandwidthRaw(newBandwidth);
  if(state == ERR_NONE) {
    SX127x::_bw = bw;
  }
  return(state);
}

int16_t SX1278::setSpreadingFactor(uint8_t sf) {
  uint8_t newSpreadingFactor;
  
  // check allowed spreading factor values
  switch(sf) {
    case 6:
      newSpreadingFactor = SX127X_SF_6;
      break;
    case 7:
      newSpreadingFactor = SX127X_SF_7;
      break;
    case 8:
      newSpreadingFactor = SX127X_SF_8;
      break;
    case 9:
      newSpreadingFactor = SX127X_SF_9;
      break;
    case 10:
      newSpreadingFactor = SX127X_SF_10;
      break;
    case 11:
      newSpreadingFactor = SX127X_SF_11;
      break;
    case 12:
      newSpreadingFactor = SX127X_SF_12;
      break;
    default:
      return(ERR_INVALID_SPREADING_FACTOR);
  }
  
  // set spreading factor and if successful, save the new setting
  int16_t state = SX1278::setSpreadingFactorRaw(newSpreadingFactor);
  if(state == ERR_NONE) {
    SX127x::_sf = sf;
  }
  return(state);
}

int16_t SX1278::setCodingRate(uint8_t cr) {
  uint8_t newCodingRate;
  
  // check allowed coding rate values
  switch(cr) {
    case 5:
      newCodingRate = SX1278_CR_4_5;
      break;
    case 6:
      newCodingRate = SX1278_CR_4_6;
      break;
    case 7:
      newCodingRate = SX1278_CR_4_7;
      break;
    case 8:
      newCodingRate = SX1278_CR_4_8;
      break;
    default:
      return(ERR_INVALID_CODING_RATE);
  }
  
  // set coding rate and if successful, save the new setting
  int16_t state = SX1278::setCodingRateRaw(newCodingRate);
  if(state == ERR_NONE) {
    SX127x::_cr = cr;
  }
  return(state);
}

int16_t SX1278::setOutputPower(int8_t power) {
  // check allowed power range
  if(!(((power >= -3) && (power <= 17)) || (power == 20))) {
    return(ERR_INVALID_OUTPUT_POWER);
  }
  
  // set mode to standby
  int16_t state = SX127x::standby();
  
  // set output power
  if(power < 2) {
    // power is less than 2 dBm, enable PA on RFO
    state |= _mod->SPIsetRegValue(SX127X_REG_PA_CONFIG, SX127X_PA_SELECT_RFO, 7, 7);
    state |= _mod->SPIsetRegValue(SX127X_REG_PA_CONFIG, SX1278_LOW_POWER | (power + 3), 6, 0);
    state |= _mod->SPIsetRegValue(SX1278_REG_PA_DAC, SX127X_PA_BOOST_OFF, 2, 0);
  } else if((power >= 2) && (power <= 17)) {
    // power is 2 - 17 dBm, enable PA1 + PA2 on PA_BOOST
    state |= _mod->SPIsetRegValue(SX127X_REG_PA_CONFIG, SX127X_PA_SELECT_BOOST, 7, 7);
    state |= _mod->SPIsetRegValue(SX127X_REG_PA_CONFIG, SX1278_MAX_POWER | (power - 2), 6, 0);
    state |= _mod->SPIsetRegValue(SX1278_REG_PA_DAC, SX127X_PA_BOOST_OFF, 2, 0);
  } else if(power == 20) {
    // power is 20 dBm, enable PA1 + PA2 on PA_BOOST and enable high power mode
    state |= _mod->SPIsetRegValue(SX127X_REG_PA_CONFIG, SX127X_PA_SELECT_BOOST, 7, 7);
    state |= _mod->SPIsetRegValue(SX127X_REG_PA_CONFIG, SX1278_MAX_POWER | (power - 5), 6, 0);
    state |= _mod->SPIsetRegValue(SX1278_REG_PA_DAC, SX127X_PA_BOOST_ON, 2, 0);
  }
  return(state);
}

int16_t SX1278::setGain(uint8_t gain) {
  // check allowed range
  if(gain > 6) {
    return(ERR_INVALID_GAIN);
  }
  
  // set mode to standby
  int16_t state = SX127x::standby();
  
  // set gain
  if(gain == 0) {
    // gain set to 0, enable AGC loop
    state |= _mod->SPIsetRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_AGC_AUTO_ON, 2, 2);
  } else {
    state |= _mod->SPIsetRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_AGC_AUTO_OFF, 2, 2);
    state |= _mod->SPIsetRegValue(SX127X_REG_LNA, (gain << 5) | SX127X_LNA_BOOST_ON);
  }
  return(state);
}

int16_t SX1278::setBandwidthRaw(uint8_t newBandwidth) {
  // set mode to standby
  int16_t state = SX127x::standby();
  
  // write register
  state |= _mod->SPIsetRegValue(SX127X_REG_MODEM_CONFIG_1, newBandwidth, 7, 4);
  return(state);
}

int16_t SX1278::setSpreadingFactorRaw(uint8_t newSpreadingFactor) {
  // set mode to standby
  int16_t state = SX127x::standby();
  
  // write registers
  if(newSpreadingFactor == SX127X_SF_6) {
    state |= _mod->SPIsetRegValue(SX127X_REG_MODEM_CONFIG_1, SX1278_HEADER_IMPL_MODE, 0, 0);
    state |= _mod->SPIsetRegValue(SX127X_REG_MODEM_CONFIG_2, SX127X_SF_6 | SX127X_TX_MODE_SINGLE | SX1278_RX_CRC_MODE_OFF, 7, 2);
    state |= _mod->SPIsetRegValue(SX127X_REG_DETECT_OPTIMIZE, SX127X_DETECT_OPTIMIZE_SF_6, 2, 0);
    state |= _mod->SPIsetRegValue(SX127X_REG_DETECTION_THRESHOLD, SX127X_DETECTION_THRESHOLD_SF_6);
  } else {
    state |= _mod->SPIsetRegValue(SX127X_REG_MODEM_CONFIG_1, SX1278_HEADER_EXPL_MODE, 0, 0);
    state |= _mod->SPIsetRegValue(SX127X_REG_MODEM_CONFIG_2, newSpreadingFactor | SX127X_TX_MODE_SINGLE | SX1278_RX_CRC_MODE_OFF, 7, 2);
    state |= _mod->SPIsetRegValue(SX127X_REG_DETECT_OPTIMIZE, SX127X_DETECT_OPTIMIZE_SF_7_12, 2, 0);
    state |= _mod->SPIsetRegValue(SX127X_REG_DETECTION_THRESHOLD, SX127X_DETECTION_THRESHOLD_SF_7_12);
  }
  return(state);
}

int16_t SX1278::setCodingRateRaw(uint8_t newCodingRate) {
  // set mode to standby
  int16_t state = SX127x::standby();
  
  // write register
  state |= _mod->SPIsetRegValue(SX127X_REG_MODEM_CONFIG_1, newCodingRate, 3, 1);
  return(state);
}

int16_t SX1278::config() {
  // configure common registers
  int16_t state = SX127x::config();
  if(state != ERR_NONE) {
    return(state);
  }
  
  // calculate symbol length and set low datarate optimization, if needed
  uint16_t base = 1;
  float symbolLength = (float)(base << _sf) / (float)_bw;
  if(symbolLength >= 0.016) {
    state = _mod->SPIsetRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_LOW_DATA_RATE_OPT_ON, 0, 0);
  } else {
    state = _mod->SPIsetRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_LOW_DATA_RATE_OPT_OFF, 0, 0);
  }
  return(state);
}