//
// Created by 23286 on 2026/2/1.
//

#include "qmath.h"

#include <math.h>

float atan_pade_33(float x) {
    // atan(x) ≈ x*(15+4x²)/(15+9x²) for |x|≤1
    float x2 = x * x;
    return x * (15.0f + 4.0f * x2) / (15.0f + 9.0f * x2);
}
// 完整的atan2实现
float fast_atan2_pade(float y, float x) {
    const float PI = 3.14159265f;
    const float PI_2 = 1.57079633f;

    if (x == 0.0f) {
        return (y > 0.0f) ? PI_2 : ((y < 0.0f) ? -PI_2 : 0.0f);
    }

    float ratio = y / x;
    float abs_ratio = fabsf(ratio);
    float angle;

    if (abs_ratio <= 1.0f) {
        angle = atan_pade_33(abs_ratio);
    } else {
        angle = PI_2 - atan_pade_33(1.0f / abs_ratio);
    }

    // 象限修正
    if (x < 0.0f) {
        angle = PI - angle;
    }
    if (y < 0.0f) {
        angle = -angle;
    }

    return angle;
}
//http://www.zybang.com/question/246f9997776f7d5cc636b10aff27a1cb.html
float qarctan(float x)  //  (-1 , +1)
{
    float t = x;
    float result = 0;
    float X2 = x * x;
    unsigned char cnt = 1;

    do
    {
        result += t / ((cnt << 1) - 1);
        t = -t;
        t *= X2;
        cnt++;
    }while(cnt <= 6);//5??
    return result;
}


