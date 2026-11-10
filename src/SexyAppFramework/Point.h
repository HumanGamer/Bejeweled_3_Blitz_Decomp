#pragma once

#include "Common.h"

namespace Sexy
{
    template<class T>
    class TPoint
    {
    public:
        T mX;
        T mY;

    public:
        TPoint(T theX, T theY) : mX(theX), mY(theY) {}
        TPoint(const TPoint<T>& theTPoint) : mX(theTPoint.mX), mY(theTPoint.mY) {}
        TPoint() : mX(0), mY(0) {}

        bool operator==(const TPoint& p) { return mX == p.mX && mY == p.mY; }
        bool operator!=(const TPoint& p) { return mX != p.mX || mY != p.mY; }

        TPoint operator+(const TPoint& p) const { return TPoint(mX + p.mX, mY + p.mY); }
        TPoint operator-(const TPoint& p) const { return TPoint(mX - p.mX, mY - p.mY); }
        TPoint operator*(const TPoint& p) const { return TPoint(mX * p.mX, mY * p.mY); }
        TPoint operator/(const TPoint& p) const { return TPoint(mX / p.mX, mY / p.mY); }
        TPoint& operator+=(const TPoint& p) { mX += p.mX; mY += p.mY; return *this; }
        TPoint& operator-=(const TPoint& p) { mX -= p.mX; mY -= p.mY; return *this; }
        TPoint& operator*=(const TPoint& p) { mX *= p.mX; mY *= p.mY; return *this; }
        TPoint& operator/=(const TPoint& p) { mX /= p.mX; mY /= p.mY; return *this; }
        TPoint operator*(T s) const { return TPoint(mX * s, mY * s); }
        TPoint operator/(T s) const { return TPoint(mX / s, mY / s); }
    };

    typedef TPoint<int> Point;
    typedef TPoint<float> FPoint;
}
