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

#ifndef __PluginParameters_FrequencyParameter_h__
#define __PluginParameters_FrequencyParameter_h__

#include <sstream>
#include <math.h>
#include "Parameter.h"

namespace teragon {

class FrequencyParameter : public Parameter {
public:
    FrequencyParameter(const ParameterString &inName,
                       ParameterValue inMinValue,
                       ParameterValue inMaxValue,
                       ParameterValue inDefaultValue) :
    Parameter(inName, inMinValue, inMaxValue, inDefaultValue) {
        logMinValue = log(inMinValue);
        range = log(inMaxValue) - log(inMinValue);
    }

    virtual ~FrequencyParameter() {}

    static ParameterString getFormattedFrequency(const ParameterValue frequency,
                                                 const unsigned int displayPrecision) {
        if(frequency >= 1000.0) {
            std::stringstream numberFormatter;
            numberFormatter.precision(displayPrecision);
            numberFormatter << std::fixed << frequency / 1000.0;
            return numberFormatter.str() + " kHz";
        }
        else {
            std::stringstream numberFormatter;
            numberFormatter.precision(displayPrecision);
            numberFormatter << std::fixed << frequency;
            return numberFormatter.str() + " Hz";
        }
    }

    virtual const ParameterString getDisplayText() const {
        return getFormattedFrequency(getValue(), getDisplayPrecision());
    }

    virtual const ParameterValue getScaledValue() const {
        return (log(getValue()) - logMinValue) / range;
    }

#if PLUGINPARAMETERS_MULTITHREADED
protected:
#endif

    virtual void setScaledValue(const ParameterValue inValue) {
        setValue(exp(inValue * range + logMinValue));
    }

private:
    double logMinValue;
    double range;
};

} // namespace teragon

#endif // __PluginParameters_FrequencyParameter_h__
