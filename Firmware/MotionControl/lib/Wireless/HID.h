#include <Arduino.h>
#include <HIDTypes.h>



// HID Descriptor
const uint8_t HIDReportMap[] = {
//  Mouse
    USAGE_PAGE(1),      0x01,           //  Generic Desktop
    USAGE(1),           0x02,           //  Mouse
    COLLECTION(1),      0x01,           //  Application
    REPORT_ID(1),       0x01,           //  1
    USAGE(1),           0x01,           //  Pointer
    COLLECTION(1),      0x00,           //  Physical
    // Buttons (Left, Right, Middle)
    USAGE_PAGE(1),      0x09,           //  Button
    USAGE_MINIMUM(1),   0x01,           //  Button 1
    USAGE_MAXIMUM(1),   0x05,           //  Button 5
    LOGICAL_MINIMUM(1), 0x00,           //  0
    LOGICAL_MAXIMUM(1), 0x01,           //  1
    REPORT_SIZE(1),     0x01,           //  1
    REPORT_COUNT(1),    0x05,           //  5
    HIDINPUT(1),        0x02,           //  INPUT (Data, Variable, Absolute)
    // Padding to make it a Byte 
    REPORT_SIZE(1),     0x03,
    REPORT_COUNT(1),    0x01,
    HIDINPUT(1),        0x03,
    // X, Y and Scroll Wheel
    USAGE_PAGE(1),      0x01,           //  Generic Desktop
    USAGE(1),           0x30,           //  USAGE (X)
    USAGE(1),           0x31,           //  USAGE (Y)
    USAGE(1),           0x38,           //  USAGE (Wheel)
    LOGICAL_MINIMUM(1), 0x81,           //  -127
    LOGICAL_MAXIMUM(1), 0x7F,           //  +127
    REPORT_SIZE(1),     0x08,           //  8
    REPORT_COUNT(1),    0x03,           //  3
    HIDINPUT(1),        0x06,           //  INPUT (Data, Variable, Relative)
    END_COLLECTION(0),
    END_COLLECTION(0),

//  Keyboard
    USAGE_PAGE(1),      0x01,           //  Generic Desktop
    USAGE(1),           0x06,           //  Keyboard
    COLLECTION(1),      0x01,           //  Application
    REPORT_ID(1),       0x02,           //  2
    // Scancode (Alt, Shift, Ctrl)
    USAGE_PAGE(1),      0x07,           //  Keyboard
    USAGE_MINIMUM(1),   0xE0,           //  LeftControl
    USAGE_MAXIMUM(1),   0xE7,           //  RightGUI
    LOGICAL_MINIMUM(1), 0x00,           //  0
    LOGICAL_MAXIMUM(1), 0x01,           //  1
    REPORT_SIZE(1),     0x01,           //  1
    REPORT_COUNT(1),    0x08,           //  8
    HIDINPUT(1),        0x02,           //  INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    // Reserved
    REPORT_COUNT(1),    0x01,
    REPORT_SIZE(1),     0x08,
    HIDINPUT(1),        0x01,
    // Keys
    REPORT_COUNT(1),    0x06,           //  6
    REPORT_SIZE(1),     0x08,           //  Byte
    LOGICAL_MINIMUM(1), 0x00,           //  0
    LOGICAL_MAXIMUM(1), 0x65,           //  101
    USAGE_PAGE(1),      0x07,           //  Keyboard
    USAGE_MINIMUM(1),   0x00,           //  0
    USAGE_MAXIMUM(1),   0x65,           //  101
    HIDINPUT(1),        0x00,           //  INPUT (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    END_COLLECTION(0)
};


// Key Mapping
const uint8_t asciihid[] = {
	0x04,           // a
	0x05,           // b
	0x06,           // c
	0x07,           // d
	0x08,           // e
	0x09,           // f
	0x0a,           // g
	0x0b,           // h
	0x0c,           // i
	0x0d,           // j
	0x0e,           // k
	0x0f,           // l
	0x10,           // m
	0x11,           // n
	0x12,           // o
	0x13,           // p
	0x14,           // q
	0x15,           // r
	0x16,           // s
	0x17,           // t
	0x18,           // u
	0x19,           // v
	0x1a,           // w
	0x1b,           // x
	0x1c,           // y
	0x1d,           // z
};


// Offsets
#define ASCII_HID_MAP_KEY_OFFSET            97

enum CONTROLKEY {
    Enter       = 0x58,
    Escape      = 0x29,
    Backspace   = 0x2A,
    Tab         = 0x2B,
    Spacebar    = 0x2C,
    RightArrow  = 0x4F,
    LeftArrow   = 0x50,
    DownArrow   = 0x51,
    UpArrow     = 0x52
};

enum MODIFIERKEY {
    LeftControl  = 0x01,  // B00000001
    LeftShift    = 0x02,  // B00000010
    LeftAlt      = 0x04,  // B00000100
    LeftGUI      = 0x08,  // B00001000
    RightControl = 0x10,  // B00010000
    RightShift   = 0x20,  // B00100000
    RightAlt     = 0x40,  // B01000000
    RightGUI     = 0x80   // B10000000
};