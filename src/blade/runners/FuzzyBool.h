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

#ifndef FUZZYBOOL_H
#define FUZZYBOOL_H

template <typename Data>
class FuzzyBool {
public:
    FuzzyBool(const FuzzyBool &other)
        : m_value(other.m_value)
    {
    }

    FuzzyBool(Data value = 0)
        : m_value(
                  value < 0 ? 0
                : value > 1 ? 1
                : value
                )
    {
    }

    operator Data() const
    {
        return m_value;
    }

    FuzzyBool operator!() const
    {
        return 1 - m_value;
    }

    FuzzyBool operator&&(const FuzzyBool &other) const
    {
        return m_value * other.m_value;
    }

    FuzzyBool operator||(const FuzzyBool &other) const
    {
        return 1 - (1 - m_value) * (1 - other.m_value);
    }

private:
    Data m_value;

    template <typename Stream>
    friend
    Stream &operator>>(Stream &stream, FuzzyBool<Data> &fbool)
    {
        return stream >> fbool.m_value;
    }

    template <typename Stream>
    friend
    Stream &operator<<(Stream &stream, FuzzyBool<Data> &fbool)
    {
        return stream << fbool.m_value;
    }
};

#endif // include guard end

