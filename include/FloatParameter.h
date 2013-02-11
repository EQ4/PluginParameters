/*
 * Copyright (c) 2013 Teragon Audio. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __FloatParameter_h__
#define __FloatParameter_h__

#include <sstream>
#include "PluginParameter.h"

namespace teragon {
class FloatParameter : public PluginParameter {
public:
  explicit FloatParameter(ParameterString name, ParameterValue minValue,
  ParameterValue maxValue, ParameterValue defaultValue) :
  PluginParameter(name, minValue, maxValue, defaultValue) {
    range = maxValue - minValue;
  }
  virtual ~FloatParameter() {}

  virtual const ParameterString getDisplayText() const {
    std::stringstream numberFormatter;
    numberFormatter.precision(1);
    numberFormatter << std::fixed << getValue();
    return numberFormatter.str();
  }
  virtual const ParameterValue getScaledValue() const {
    return (getValue() - getMinValue()) / range;
  }
  virtual void setScaledValue(const ParameterValue value) {
    setValue(value * range + getMinValue());
  }

private:
  ParameterValue range;
};
}

#endif