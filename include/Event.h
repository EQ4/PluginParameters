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

#ifndef __EVENT_H__
#define	__EVENT_H__

#if ENABLE_MULTITHREADED
#include "PluginParameter.h"
#include "StringParameter.h"

namespace teragon {

class Event {
public:
  Event(PluginParameter* p, ParameterValue v,
    bool realtime = false, ParameterObserver* s = NULL) :
    parameter(p), value(v), isRealtime(realtime), sender(s) {}
  virtual ~Event() {}

  virtual void apply() { parameter->setValue(value); }

  PluginParameter* parameter;
  const ParameterValue value;
  bool isRealtime;
  const ParameterObserver* sender;

private:
  // Disallow assignment operator
  Event& operator=(const Event&) { return *this; }
};

class ScaledEvent : public Event {
public:
  ScaledEvent(PluginParameter* p, ParameterValue v,
    bool realtime = false, ParameterObserver* s = NULL) :
  Event(p, v, realtime, s) {}
  virtual ~ScaledEvent() {}

  virtual void apply() { parameter->setScaledValue(value); }
};

class StringEvent : public Event {
public:
  StringEvent(StringParameter* p, ParameterString v,
    bool realtime = false, ParameterObserver* s = NULL) :
  Event(dynamic_cast<PluginParameter*>(p), 0, realtime, s),
    stringParameter(p), stringValue(v) {}
  virtual ~StringEvent() {}

  virtual void apply() { stringParameter->setValue(stringValue); }

  StringParameter* stringParameter;
  const ParameterString stringValue;
};
} // namespace teragon

#endif // ENABLE_MULTITHREADED
#endif // __EVENT_H__
