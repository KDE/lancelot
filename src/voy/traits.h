/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_TRAITS_H
#define VOY_TRAITS_H

// STL
#include <type_traits>

// Some meta-programming utility functions and meta-functions,
// including the detection idiom - the poor man's concepts

#define voy_concept inline constexpr bool

#define voy_require(Condition) typename std::enable_if_t<Condition, int> = 0

#define voy_assert_value_type(Type)                                            \
    static_assert(std::is_same_v<Type, ::voy::traits::remove_cvref_t<Type>>,   \
                  "This needs to be a value type")

namespace voy::traits {

// Meta-function to force the compiler to print out the exact type passed
// to it by creating a compilation error because the type is not implemented
template <typename... Types>
struct print_types;

// Meta-function that always returns false. It is useful for marking a
// particular if-constexpr branch to be erroneous
template <typename T>
voy_concept always_fail = false;

#define voy_fail(Type, Msg) static_assert(::voy::traits::always_fail<Type>, Msg)


// C++20 Stuff

// Meta-function which strips out the reference and cv qualifiers from the type
template <typename T>
using remove_cvref_t = std::remove_reference_t<std::remove_cv_t<T>>;


// The detection idiom -- static introspection made easy

struct nonesuch {
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(const nonesuch&) = delete;
    nonesuch& operator=(const nonesuch&) = delete;
};

namespace detail {
    template < typename Default
             , typename Void
             , template <typename...> typename Operation
             , typename... Args
             >
    struct detector {
        using value_t = std::false_type;
        using type = Default;
    };

    template < typename Default
             , template <typename...> typename Operation
             , typename... Args
             >
    struct detector < Default
                    , std::void_t<Operation<Args...>>
                    , Operation
                    , Args...
                    > {
        using value_t = std::true_type;
        using type = Operation<Args...>;
    };
} // namespace detail


template < template <typename...> typename Operation
         , typename... Args
         >
using is_detected = typename detail::detector < nonesuch
                                               , void
                                               , Operation
                                               , Args...
                                               >::value_t;


template < template <typename...> typename Operation
         , typename... Args
         >
voy_concept is_detected_v = is_detected<Operation, Args...>::value;


template < template <typename...> typename Operation
         , typename... Args
         >
using is_detected_t = typename is_detected<Operation, Args...>::type;


template < template <typename...> typename Operation
         , typename... Args
         >
using detected_t = typename detail::detector < nonesuch
                                             , void
                                             , Operation
                                             , Args...>::type;


} // namespace voy::traits

#endif // include guard

