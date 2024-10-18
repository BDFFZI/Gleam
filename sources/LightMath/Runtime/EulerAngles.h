#pragma once
#include<string>

struct EulerAngles
{
    float x;
    float y;
    float z;

    EulerAngles(float x, float y, float z);
    EulerAngles(float value);

    EulerAngles GetNormalized() const;
};

std::string to_string(EulerAngles value);
EulerAngles operator+(EulerAngles left, EulerAngles right);
EulerAngles operator-(EulerAngles left, EulerAngles right);
EulerAngles operator*(EulerAngles left, EulerAngles right);
EulerAngles operator/(EulerAngles left, EulerAngles right);
bool operator==(EulerAngles left, EulerAngles right);
