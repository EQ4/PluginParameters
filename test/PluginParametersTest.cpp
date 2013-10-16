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

#include <stdio.h>
#include <stdlib.h>
#include "TestRunner.h"
#include "PluginParameters.h"

extern int runMultithreadedTests();

namespace teragon {

////////////////////////////////////////////////////////////////////////////////
// Observers
////////////////////////////////////////////////////////////////////////////////

class TestObserver : public PluginParameterObserver {
public:
  TestObserver(bool &inB) : b(inB) {}
#if ENABLE_MULTITHREADED
  bool isRealtimePriority() const { return true; }
#endif
  void onParameterUpdated(const PluginParameter* parameter) {
    b = true;
  }
private:
  bool& b;
};

class TestCounterObserver : public PluginParameterObserver {
public:
  TestCounterObserver() : count(0) {}
#if ENABLE_MULTITHREADED
  bool isRealtimePriority() const { return true; }
#endif
  void onParameterUpdated(const PluginParameter* parameter) {
    count++;
  }
  int count;
};

class BooleanParameterListener : public PluginParameterObserver {
public:
  BooleanParameterListener() : PluginParameterObserver(), myValue(false) {}
  virtual ~BooleanParameterListener() {}
  bool myValue;
#if ENABLE_MULTITHREADED
  bool isRealtimePriority() const { return true; }
#endif
  void onParameterUpdated(const PluginParameter *parameter) {
    myValue = parameter->getValue();
  }
};

class StringParameterListener : public PluginParameterObserver {
public:
  ParameterString myValue;
#if ENABLE_MULTITHREADED
  bool isRealtimePriority() const { return true; }
#endif
  void onParameterUpdated(const PluginParameter* parameter) {
    myValue = parameter->getDisplayText();
  }
};

////////////////////////////////////////////////////////////////////////////////
// Tests
////////////////////////////////////////////////////////////////////////////////

class PluginParametersTests {
public:
static bool testCreateBoolParameter() {
  BooleanParameter p("test");
  ASSERT_FALSE(p.getValue());
  ASSERT_EQUALS(0.0, p.getScaledValue());
  ASSERT_STRING("test", p.getName());
  ASSERT_EQUALS(0.0, p.getDisplayValue());
  ASSERT_STRING("false", p.getDisplayText());
  ASSERT_STRING("test", p.getSafeName());
  return true;
}

static bool testSetBoolParameter() {
  BooleanParameter p("test");
  ASSERT_FALSE(p.getValue());
  p.setValue(1.0);
  ASSERT(p.getValue());
  return true;
}

static bool testSetBoolParameterWithListener() {
  BooleanParameter *p = new BooleanParameter("test");
  BooleanParameterListener l;
  p->addObserver(&l);
#if ENABLE_MULTITHREADED
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(p));
  s.set(p, true, l.isRealtimePriority());
  s.processRealtimeEvents();
  ASSERT(l.myValue);
#else
  p->setValue(true);
  ASSERT(l.myValue);
  delete p;
#endif
  return true;
}

static bool testCreateDecibelParameter() {
  DecibelParameter p("test", -60.0, 3.0, 0.0);
  ASSERT_EQUALS(0.707739, p.getScaledValue());
  ASSERT_EQUALS(1.0, p.getValue());
  ASSERT_STRING("0.0 dB", p.getDisplayText());
  return true;
}

static bool testSetDecibelParameter() {
  DecibelParameter p("test", -60.0, 3.0, -10.0);
  ASSERT_STRING("-10.0 dB", p.getDisplayText());
  p.setValue(1.0);
  ASSERT_EQUALS(0.707739, p.getScaledValue());
  ASSERT_EQUALS(1.0, p.getValue());
  ASSERT_STRING("0.0 dB", p.getDisplayText());
  p.setScaledValue(0.5);
  ASSERT_EQUALS(0.5, p.getScaledValue());
  ASSERT_EQUALS(0.706769, p.getValue());
  ASSERT_STRING("-3.0 dB", p.getDisplayText());
  return true;
}

static bool testCreateFloatParameter() {
  FloatParameter p("test", 0.0, 50.0, 25.0);
  ASSERT_EQUALS(25.0, p.getValue());
  ASSERT_EQUALS(0.5, p.getScaledValue());
  ASSERT_STRING("25.0", p.getDisplayText());
  return true;
}

static bool testSetFloatParameter() {
  FloatParameter p("test", 0.0, 60.0, 0.0);
  p.setValue(30.0);
  ASSERT_EQUALS(30.0, p.getValue());
  ASSERT_EQUALS(0.5, p.getScaledValue());
  p.setScaledValue(0.25);
  ASSERT_EQUALS(15.0, p.getValue());
  ASSERT_EQUALS(0.25, p.getScaledValue());
  return true;
}

static bool testCreateFrequencyParameter() {
  FrequencyParameter p("test", 20.0, 20000.0, 10000.0);
  ASSERT_EQUALS(10000.0, p.getValue());
  ASSERT_EQUALS(0.899657, p.getScaledValue());
  ASSERT_STRING("10.0 kHz", p.getDisplayText());
  return true;
}

static bool testSetFrequencyParameter() {
  FrequencyParameter p("test", 20.0, 20000.0, 10000.0);
  p.setValue(666.0);
  ASSERT_EQUALS(666.0, p.getValue());
  ASSERT_EQUALS(0.507481, p.getScaledValue());
  p.setScaledValue(0.75);
  ASSERT_EQUALS(3556.559, p.getValue());
  ASSERT_EQUALS(0.75, p.getScaledValue());
  return true;
}

static bool testCreateIntegerParameter() {
  IntegerParameter p("test", 0, 60, 15);
  ASSERT_EQUALS(15.0, p.getValue());
  ASSERT_EQUALS(0.25, p.getScaledValue());
  ASSERT_STRING("15", p.getDisplayText());
  return true;
}

static bool testSetIntegerParameter() {
  IntegerParameter p("test", 0, 60, 15);
  p.setValue(30);
  ASSERT_EQUALS(30.0, p.getValue());
  ASSERT_EQUALS(0.5, p.getScaledValue());
  p.setScaledValue(0.75);
  ASSERT_EQUALS(45.0, p.getValue());
  ASSERT_EQUALS(0.75, p.getScaledValue());
  return true;
}

static bool testCreateStringParameter() {
  StringParameter p("test", "whatever");
  ASSERT_EQUALS(0.0, p.getValue());
  ASSERT_STRING("whatever", p.getDisplayText());
  return true;
}

static bool testSetStringParameter() {
  StringParameter p("test", "whatever");
  ASSERT_EQUALS(0.0, p.getValue());
  ASSERT_STRING("whatever", p.getDisplayText());
  p.setValue("something");
  ASSERT_STRING("something", p.getDisplayText());
  return true;
}

static bool testSetStringParameterWithListener() {
  StringParameter *p = new StringParameter("test", "whatever");
  StringParameterListener l;
  p->addObserver(&l);
#if ENABLE_MULTITHREADED
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(p));
  s.set(p, "something", l.isRealtimePriority());
  s.processRealtimeEvents();
  ASSERT_STRING("something", l.myValue);
#else
  p->setValue("something");
  ASSERT_STRING("something", l.myValue);
  delete p;
#endif
  return true;
}

static bool testCreateVoidParameter() {
  VoidParameter *p = new VoidParameter("test");
  ASSERT_EQUALS(0.0, p->getValue());
  TestCounterObserver l;
  p->addObserver(&l);
#if ENABLE_MULTITHREADED
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(p));
  s.set(p, 0, l.isRealtimePriority());
  s.processRealtimeEvents();
  ASSERT_INT_EQUALS(1, l.count);
#else
  p->setValue();
  ASSERT_INT_EQUALS(1, l.count);
  delete p;
#endif
  return true;
}

static bool testCreateParameterWithBadName() {
  // NOTE: This test will succeed, I'd rather not throw from the ctor!
  // So use your head and don't make any weird parameter names. Just know
  // that the library isn't going to protect you from yourself. :)
  BooleanParameter p("");
  return true;
}

static bool testCreateParameterWithBadRange() {
  // NOTE: This test will also succeed, PluginParameters doesn't check for bad ranges
  // Just be aware of this behavior rather than trying to abuse the library.
  IntegerParameter p("bad", 100.0, 0.0, 300.0);
  return true;
}

static bool testAddParameterToSet() {
  PluginParameterSet s;
  PluginParameter* _p1;
  PluginParameter* _p2;

  _p1 = s.add(new BooleanParameter("Parameter 1"));
  ASSERT_NOT_NULL(_p1);
  _p2 = s.add(new BooleanParameter("Parameter 2"));
  ASSERT_NOT_NULL(_p2);
  ASSERT_INT_EQUALS(2, s.size());
  ASSERT_STRING("Parameter 1", s.get(0)->getName());
  ASSERT_STRING("Parameter 1", _p1->getName());
  ASSERT_STRING("Parameter 2", s.get(1)->getName());
  ASSERT_STRING("Parameter 2", _p2->getName());
  return true;
}

static bool testAddNullParameterToSet() {
  PluginParameterSet s;
  ASSERT_IS_NULL(s.add(NULL));
  ASSERT_INT_EQUALS(0, s.size());
  return true;
}

static bool testAddDuplicateParameterToSet() {
  BooleanParameter *p = new BooleanParameter("test");
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(p));
  ASSERT_IS_NULL(s.add(p));
  ASSERT_INT_EQUALS(1, s.size());
  return true;
}

static bool testAddDuplicateSafeNameParameterToSet() {
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter1")));
  ASSERT_IS_NULL(s.add(new BooleanParameter("Parameter 1")));
  ASSERT_INT_EQUALS(1, s.size());
  return true;
}

static bool testClearParameterSet() {
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter1")));
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter2")));
  ASSERT_INT_EQUALS(2, s.size());
  s.clear();
  ASSERT_INT_EQUALS(0, s.size());
  return true;
}

static bool testGetParameterByName() {
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter 1")));
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter 2")));
  ASSERT_INT_EQUALS(2, s.size());
  PluginParameter *pe = s.get("Parameter 2");
  ASSERT_NOT_NULL(pe);
  ASSERT_STRING("Parameter 2", pe->getName());
  return true;
}

static bool testGetParameterByIndex() {
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter 1")));
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter 2")));
  ASSERT_INT_EQUALS(2, s.size());
  ASSERT_STRING("Parameter 2", s.get(1)->getName());
  return true;
}

static bool testGetParameterByNameOperator() {
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter 1")));
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter 2")));
  ASSERT_INT_EQUALS(2, s.size());
  ASSERT_STRING("Parameter 2", s["Parameter 2"]->getName());
  return true;
}

static bool testGetParameterByIndexOperator() {
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter 1")));
  ASSERT_NOT_NULL(s.add(new BooleanParameter("Parameter 2")));
  ASSERT_INT_EQUALS(2, s.size());
  ASSERT_STRING("Parameter 2", s[1]->getName());
  return true;
}

static bool testGetSafeName() {
  BooleanParameter p("hi there");
  ASSERT_STRING("hithere", p.getSafeName());
  return true;
}

static bool testAddObserver() {
  bool b = false;
  BooleanParameter *p = new BooleanParameter("test");
  TestObserver t(b);
  p->addObserver(&t);
#if ENABLE_MULTITHREADED
  PluginParameterSet s;
  ASSERT_NOT_NULL(s.add(p));
  s.set(p, 1.0, t.isRealtimePriority());
  s.processRealtimeEvents();
  ASSERT(b);
#else
  p->setValue(1.0);
  ASSERT(b);
  delete p;
#endif
  return true;
}

static bool testRemoveObserver() {
  bool b = false;
  BooleanParameter p("test");
  TestObserver t(b);
  p.addObserver(&t);
  p.removeObserver(&t);
  p.setValue(1.0);
  ASSERT_FALSE(b);
  return true;
}

static bool testShouldNotNotifyForSameValue() {
  BooleanParameter p("test", false);
  TestCounterObserver l;
  p.addObserver(&l);
  p.setValue(p.getValue());
  ASSERT_INT_EQUALS(0, l.count);
  return true;
}

static bool testParameterType() {
  BooleanParameter p("test");
  ASSERT_INT_EQUALS(0, p.getType());
  p.setType(1234);
  ASSERT_INT_EQUALS(1234, p.getType());
  return true;
}

static bool testGetMinValue() {
  FloatParameter p("test", 0.123, 1.23, 1.00);
  ASSERT_EQUALS(0.123, p.getMinValue());
  return true;
}

static bool testGetMaxValue() {
  FloatParameter p("test", 0.123, 1.23, 1.00);
  ASSERT_EQUALS(1.23, p.getMaxValue());
  return true;
}

static bool testGetDefaultValue() {
  FloatParameter p("test", 0.123, 1.23, 1.00);
  ASSERT_EQUALS(1.0, p.getDefaultValue());
  return true;
}

static bool testSetParameterUnit() {
  FloatParameter p("test", 0.0, 1.0, 0.0);
  ASSERT_STRING("0.0", p.getDisplayText());
  p.setUnit("foo");
  ASSERT_STRING("0.0 foo", p.getDisplayText());
  return true;
}

static bool testSetPrecision() {
  FloatParameter p("test", 0.0, 1.0, 0.123456);
  ASSERT_STRING("0.1", p.getDisplayText());
  p.setDisplayPrecision(3);
  ASSERT_STRING("0.123", p.getDisplayText());
  // Rounding!
  p.setDisplayPrecision(5);
  ASSERT_STRING("0.12346", p.getDisplayText());
  return true;
}
};
} // namespace teragon

using namespace teragon;

////////////////////////////////////////////////////////////////////////////////
// Run test suite
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
  gNumFailedTests = 0;

  ADD_TEST("CreateBoolParameter", PluginParametersTests::testCreateBoolParameter());
  ADD_TEST("SetBoolParameter", PluginParametersTests::testSetBoolParameter());
  ADD_TEST("SetBoolParameterWithListener", PluginParametersTests::testSetBoolParameterWithListener());

  ADD_TEST("CreateDecibelParameter", PluginParametersTests::testCreateDecibelParameter());
  ADD_TEST("SetDecibelParameter", PluginParametersTests::testSetDecibelParameter());

  ADD_TEST("CreateFloatParameter", PluginParametersTests::testCreateFloatParameter());
  ADD_TEST("SetFloatParameter", PluginParametersTests::testSetFloatParameter());

  ADD_TEST("CreateFrequencyParameter", PluginParametersTests::testCreateFrequencyParameter());
  ADD_TEST("SetFrequencyParameter", PluginParametersTests::testSetFrequencyParameter());

  ADD_TEST("CreateIntegerParameter", PluginParametersTests::testCreateIntegerParameter());
  ADD_TEST("SetIntegerParameter", PluginParametersTests::testSetIntegerParameter());

  ADD_TEST("CreateStringParameter", PluginParametersTests::testCreateStringParameter());
  ADD_TEST("SetStringParameter", PluginParametersTests::testSetStringParameter());
  ADD_TEST("SetStringParameterWithListener", PluginParametersTests::testSetStringParameterWithListener());

  ADD_TEST("CreateVoidParameter", PluginParametersTests::testCreateVoidParameter());

  ADD_TEST("CreateParameterWithBadName", PluginParametersTests::testCreateParameterWithBadName());
  ADD_TEST("CreateParameterWithBadRange", PluginParametersTests::testCreateParameterWithBadRange());

  ADD_TEST("AddParameterToSet", PluginParametersTests::testAddParameterToSet());
  ADD_TEST("AddNullParameterToSet", PluginParametersTests::testAddNullParameterToSet());
  ADD_TEST("AddDuplicateParameterToSet", PluginParametersTests::testAddDuplicateParameterToSet());
  ADD_TEST("AddDuplicateSafeNameParameterToSet", PluginParametersTests::testAddDuplicateSafeNameParameterToSet());

  ADD_TEST("ClearParameterSet", PluginParametersTests::testClearParameterSet());
  ADD_TEST("GetParameterByName", PluginParametersTests::testGetParameterByName());
  ADD_TEST("GetParameterByIndex", PluginParametersTests::testGetParameterByIndex());
  ADD_TEST("GetParameterByNameOperator", PluginParametersTests::testGetParameterByNameOperator());
  ADD_TEST("GetParameterByIndexOperator", PluginParametersTests::testGetParameterByIndexOperator());

  ADD_TEST("GetSafeName", PluginParametersTests::testGetSafeName());
  ADD_TEST("AddObserver", PluginParametersTests::testAddObserver());
  ADD_TEST("RemoveObserver", PluginParametersTests::testRemoveObserver());
  ADD_TEST("ShouldNotNotifyForSameValue", PluginParametersTests::testShouldNotNotifyForSameValue());

  ADD_TEST("ParameterType", PluginParametersTests::testParameterType());
  ADD_TEST("GetMinValue", PluginParametersTests::testGetMinValue());
  ADD_TEST("GetMaxValue", PluginParametersTests::testGetMaxValue());
  ADD_TEST("GetDefaultValue", PluginParametersTests::testGetDefaultValue());
  ADD_TEST("SetParameterUnit", PluginParametersTests::testSetParameterUnit());
  ADD_TEST("SetPrecision", PluginParametersTests::testSetPrecision());

  runMultithreadedTests();

  if(gNumFailedTests > 0) {
    printf("\nFAILED %d tests\n", gNumFailedTests);
  }
  else {
    printf("\nAll tests passed\n");
  }

  return gNumFailedTests;
}
