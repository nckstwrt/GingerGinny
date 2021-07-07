#ifndef _HELPER_H
#define _HELPER_H
#include <memory>
#include <string>
#include <stdexcept>
#include <functional>
#include <math.h>
using namespace std;

class Helper
{
public:
    template<typename ... Args>
    static std::string string_format(const std::string& format, Args ... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
        if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
        auto size = static_cast<size_t>(size_s);
        auto buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }

    static bool string_replace(std::string& str, const std::string& from, const std::string& to)
    {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

    static void CalcLine(int x0, int y0, int x1, int y1, std::function<void(int, int, int, int)> callback)
    {
        int sx = 0;
        int sy = 0;

        int dx = abs(x1 - x0);
        sx = x0 < x1 ? 1 : -1;
        int dy = -1 * abs(y1 - y0);
        sy = y0 < y1 ? 1 : -1;
        int err = dx + dy, e2; 

        for (;;)
        {
            if (x0 == x1 && y0 == y1)
                break;
            e2 = 2 * err;
            if (e2 >= dy)
            {
                err += dy;
                x0 += sx;
                callback(x0, y0, sx, 0);
            }
            if (e2 <= dx)
            {
                err += dx;
                y0 += sy;
                callback(x0, y0, 0, sy);
            }
        }
    }
};

class Point
{
public:
    Point() : Point(-1, -1)
    {
    }

    Point(int x, int y) : x(x), y(y)
    {
    }

    bool operator== (const Point& rhs)
    {
        return (x == rhs.x && y == rhs.y);
    }

    Point operator+(const Point& rhs)
    {
        return { x + rhs.x, y + rhs.y };
    }

    int x;
    int y;
};

class Rect
{
public:
    Rect() : x(0), y(0), width(0), height(0)
    {
    }

    Rect(Point p1, Point p2)
    {
        x = p1.x;
        y = p1.y;
        width = p2.x - p1.x;
        height = p2.y - p1.y;
    }

    Rect(int x, int y, int width, int height) :
        x(x), y(y), width(width), height(height)
    {
    }
    
    Rect(const Rect &rhs)
    {
        x = rhs.x;
        y = rhs.y;
        width = rhs.width;
        height = rhs.height;
    }
    
    bool IntersectsRect(const Rect& rhs)
    {
        return ((x < (rhs.x + rhs.width)) && ((x + width) > rhs.x) && (y < (rhs.y + rhs.height)) && ((y + height) > rhs.y));
    }

    bool ContainsPoint(int pointX, int pointY)
    {
        return (pointX >= x && pointX <= (x + width) && pointY >= y && pointY <= (y + height));
    }

    bool operator== (const Rect& rhs)
    {
        return (x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height);
    }

    void DivideBy(int tileSize)
    {
        x /= tileSize;
        y /= tileSize;
        width /= tileSize;
        height /= tileSize;
    }

    int x;
    int y;
    int width;
    int height;
};

class Circle
{
public:
    Circle(int originX, int originY, int radius) : 
        originX(originX), originY(originY), radius(radius)
    {
    }

    bool ContainsPoint(int x, int y)
    {
        return ((x - originX) * (x - originX) + (y - originY) * (y - originY) <= radius * radius);
    }

    vector<Point> GetAllCirclePoints()
    {
        vector<Point> points;
        for (int x = -radius; x < radius; x++)
        {
            int height = (int)sqrt(radius * radius - x * x);

            for (int y = -height; y < height; y++)
            {
                points.push_back({ x + originX, y + originY });
            }
        }
        return points;
    }

    int originX;
    int originY;
    int radius;
};

#endif