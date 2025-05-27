#pragma once
#include "event_bus.h"

enum message_topic_t { TEMP = 1, COMMAND = 2 };

struct Temp {
    float value;
    friend void toJson(JsonVariant v, Temp const &t) { v["value"] = t.value; }
    void fromJson(JsonObjectConst o) { value = o["value"].as<float>(); }
};

struct Command {
    float lx, ly, rx, ry, h, s, s1;
    friend void toJson(JsonVariant v, Command const &c) {
        v["lx"] = c.lx;
        v["ly"] = c.ly;
        v["rx"] = c.rx;
        v["ry"] = c.ry;
        v["h"] = c.h;
        v["s"] = c.s;
        v["s1"] = c.s1;
    }

    void fromJson(JsonObjectConst o) {
        lx = o["lx"].as<int>();
        ly = o["ly"].as<int>();
        rx = o["rx"].as<int>();
        ry = o["ry"].as<int>();
        h = o["h"].as<int>();
        s = o["s"].as<int>();
        s1 = o["s1"].as<int>();
    }
};
