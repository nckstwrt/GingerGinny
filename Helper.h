#ifndef _HELPER_H
#define _HELPER_H
#include <memory>
#include <string>
#include <stdexcept>
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

    int x;
    int y;
};

#endif