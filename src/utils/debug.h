/*
 *   Copyright (C) 2016 Ivan Čukić <ivan.cukic(at)kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.
 *   If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_DEBUG_H
#define UTILS_DEBUG_H

#include <iostream>
#include <string>

// Black       0;30     Dark Gray     1;30
// Blue        0;34     Light Blue    1;34
// Green       0;32     Light Green   1;32
// Cyan        0;36     Light Cyan    1;36
// Red         0;31     Light Red     1;31
// Purple      0;35     Light Purple  1;35
// Brown       0;33     Yellow        1;33
// Light Gray  0;37     White         1;37


#define COLOR_RESET "\033[0m"

#define DEBUG_COLOR_GRAY   "\033[30;1m"
#define DEBUG_COLOR_RED    "\033[31;1m"
#define DEBUG_COLOR_GREEN  "\033[32;1m"
#define DEBUG_COLOR_YELLOW "\033[33;1m"
#define DEBUG_COLOR_BLUE   "\033[34;1m"
#define DEBUG_COLOR_PURPLE "\033[35;1m"
#define DEBUG_COLOR_CYAN   "\033[36;1m"

#define DEBUG_COLOR_SLIM_BLUE   "\033[34m"
#define DEBUG_COLOR_SLIM_GREEN  "\033[32m"
#define DEBUG_COLOR_SLIM_CYAN   "\033[36m"
#define DEBUG_COLOR_SLIM_RED    "\033[31m"
#define DEBUG_COLOR_SLIM_PURPLE "\033[35m"
#define DEBUG_COLOR_SLIM_YELLOW "\033[33m"
#define DEBUG_COLOR_SLIM_GRAY   "\033[30m"

#define DEBUG_COLOR_VALUE  COLOR_RESET DEBUG_COLOR_SLIM_BLUE

namespace debug {

template <typename T>
struct pretty_ptr {
    pretty_ptr(T* ptr)
        : ptr{ptr}
    {
    }

    T *ptr;
};


template <typename T>
std::ostream& operator<< (std::ostream& out, const pretty_ptr<T>& pretty)
{
    short ptrhash = ((intptr_t)pretty.ptr / 32) % 8;
    // std::string basecol("\033[30m");
    std::string basecol(DEBUG_COLOR_GRAY);
    basecol[3] = '0' + ptrhash;

    return out << basecol << '[' << (void*)pretty.ptr << ']' << COLOR_RESET;
}


template <typename T>
std::ostream& debug_for(T* ptr)
{
    return std::cerr << pretty_ptr{ptr} << ' ';
}


enum class color {
    gray,
    red,
    green,
    yellow,
    blue,
    purple,
    cyan
};


template <typename = void>
class debug_impl {
public:
    template <typename T>
    debug_impl& operator<< (const T& value)
    {
        m_out << value;
        return *this;
    }

    debug_impl& operator<< (color c)
    {
        m_out << ( c == color::gray   ? DEBUG_COLOR_GRAY
                 : c == color::red    ? DEBUG_COLOR_RED
                 : c == color::green  ? DEBUG_COLOR_GREEN
                 : c == color::yellow ? DEBUG_COLOR_YELLOW
                 : c == color::blue   ? DEBUG_COLOR_BLUE
                 : c == color::purple ? DEBUG_COLOR_PURPLE
                 : c == color::cyan   ? DEBUG_COLOR_CYAN
                 : COLOR_RESET );
        return *this;
    }

    debug_impl()
        : m_out{std::cerr}
    {
    }

    debug_impl(color c)
        : m_out{std::cerr}
    {
        operator<<(c);
    }

    ~debug_impl()
    {
        m_out << COLOR_RESET << std::endl;
    }


private:
    std::ostream& m_out;
};


inline debug_impl<> out()
{
    return debug_impl<>{};
}


inline debug_impl<> out(color c)
{
    return debug_impl<>{c};
}


} // namespace debug

#endif // include guard end

