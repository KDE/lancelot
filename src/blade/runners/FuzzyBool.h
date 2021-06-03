/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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

