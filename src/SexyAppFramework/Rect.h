#pragma once

#include "Common.h"
#include "Point.h"

#include <list>

namespace Sexy
{
    template <class T>
    class TRect
    {
    public:
        T mX;
        T mY;
        T mWidth;
        T mHeight;

    public:
        TRect(T theX, T theY, T theWidth, T theHeight) : mX(theX), mY(theY), mWidth(theWidth), mHeight(theHeight) {}
        TRect(const TRect<T>& theTRect) : mX(theTRect.mX), mY(theTRect.mY), mWidth(theTRect.mWidth), mHeight(theTRect.mHeight) {}
        TRect() : mX(0), mY(0), mWidth(0), mHeight(0) {}

        bool Intersects(const TRect<T>& theTRect) const
        {
            return !(theTRect.mX + theTRect.mWidth <= mX ||
                     theTRect.mY + theTRect.mHeight <= mY ||
                     theTRect.mX >= mX + mWidth ||
                     theTRect.mY >= mY + mHeight);
        }

        TRect<T> Intersection(const TRect<T>& theTRect) const
        {
            T x1 = max(mX, theTRect.mX);
            T x2 = min(mX + mWidth, theTRect.mX + theTRect.mWidth);
            T y1 = max(mY, theTRect.mY);
            T y2 = min(mY + mHeight, theTRect.mY + theTRect.mHeight);
            if (((x2 - x1) < 0) || ((y2 - y1) < 0))
                return TRect<T>(0,0,0,0);
            else
                return TRect<T>(x1, y1, x2 - x1, y2 - y1);
        }

        TRect<T> Union(const TRect<T>& theTRect)
        {
            T x1 = min(mX, theTRect.mX);
            T x2 = max(mX + mWidth, theTRect.mX + theTRect.mWidth);
            T y1 = min(mY, theTRect.mY);
            T y2 = max(mY + mHeight, theTRect.mY + theTRect.mHeight);
            return TRect<T>(x1, y1, x2 - x1, y2 - y1);
        }

        bool Contains(T theX, T theY) const
        {
            return theX >= mX && theX < mX + mWidth &&
                   theY >= mY && theY < mY + mHeight;
        }

        bool Contains(const TPoint<T>& thePoint) const
        {
            return thePoint.mX >= mX && thePoint.mX < mX + mWidth &&
                   thePoint.mY >= mY && thePoint.mY < mY + mHeight;
        }

        void Offset(T theX, T theY)
        {
            mX += theX;
            mY += theY;
        }

        void Offset(const TPoint<T>& thePoint)
        {
            mX += thePoint.mX;
            mY += thePoint.mY;
        }

        TRect Inflate(T theX, T theY)
        {
            mX -= theX;
            mWidth += theX * 2;
            mY -= theY;
            mHeight += theY * 2;

            return *this;
        }

        bool operator==(const TRect<T>& theRect) const
        {
            return mX == theRect.mX && mY == theRect.mY && mWidth == theRect.mWidth && mHeight == theRect.mHeight;
        }

        RECT ToRECT() const
        {
            RECT aRect = {mX, mY, mX + mWidth, mY + mHeight};
            return aRect;
        }
    };

    typedef TRect<int> Rect;
    typedef TRect<float> FRect;
}
