#pragma once
#include "event_bus.h"

struct Temp : JsonSerializable<Temp> {
    float value;
    void toJson(JsonObject& o) const { o["value"] = value; }
    void fromJson(JsonObjectConst o) { value = o["value"].as<float>(); }
};

struct Command : JsonSerializable<Command> {
    int stop;
    float lx, ly, rx, ry, h, s, s1;
    void toJson(JsonObject& o) const {
        o["stop"] = stop;
        o["lx"] = lx;
        o["ly"] = ly;
        o["rx"] = rx;
        o["ry"] = ry;
        o["h"] = h;
        o["s"] = s;
        o["s1"] = s1;
    }
    void fromJson(JsonObjectConst o) {
        stop = o["stop"].as<int>();
        lx = o["lx"].as<float>();
        ly = o["ly"].as<float>();
        rx = o["rx"].as<float>();
        ry = o["ry"].as<float>();
        h = o["h"].as<float>();
        s = o["s"].as<float>();
        s1 = o["s1"].as<float>();
    }
};
