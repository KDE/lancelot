/*
 *   Copyright (C) 2019 Ivan Čukić <ivan.cukic(at)kde.org>
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

#ifndef VOY_WRAPPERS_QT_CONNECT_H
#define VOY_WRAPPERS_QT_CONNECT_H

// STL
#include <functional>

// Qt
#include <QObject>
#include <QMetaObject>
#include <QDebug>

// Self
#include "../../utils.h"
#include "../../traits.h"
#include "../../dsl/node_tags.h"

namespace voy::qt {

using voy::utils::non_copyable;

using voy::dsl::continuator_base,
      voy::dsl::source_node_tag;

namespace detail {

    template <typename T, typename Object, typename Signal>
    class signal_impl: non_copyable {
        voy_assert_value_type(T);

    public:
        using node_category = source_node_tag;

        explicit signal_impl(Object* sender, Signal signal)
            : m_sender{sender}
            , m_signal{std::move(signal)}
        {
        }

        template <typename Cont>
        class node: public continuator_base<Cont>, non_copyable {
            using base = continuator_base<Cont>;

        public:
            node(Object* sender, Signal signal, Cont cont)
                : base{std::move(cont)}
                , m_sender{sender}
                , m_signal{std::move(signal)}
            {
            }

            void init()
            {
                qDebug() << "Calling init";
                base::init();

                if constexpr (std::is_same_v<void, T>) {
                    m_connection = QObject::connect(
                            m_sender, m_signal,
                            [this] (auto value) {
                                // TODO: This needs to be async
                                base::emit(std::move(value));
                            });
                } else {
                    m_connection = QObject::connect(
                            m_sender, m_signal,
                            [this] (T value) {
                                // TODO: This needs to be async
                                base::emit(std::move(value));
                            });
                }

                QObject::connect(
                        m_sender, &QObject::destroyed,
                        [this] {
                            base::notify_ended();
                        });
            }

        private:
            QMetaObject::Connection m_connection;
            Object* m_sender;
            Signal m_signal;
        };


        template <typename Cont>
        auto with_continuation(Cont&& cont) &&
        {
            return node<Cont>(m_sender, std::move(m_signal), voy_fwd(cont));
        }

    private:
        Object* m_sender;
        Signal m_signal;

    };



    template <typename Object, typename Slot>
    class slot_impl: non_copyable {
    public:
        using node_category = sink_node_tag;

        explicit slot_impl(Object* object, Slot slot)
            : m_receiver(object)
            , m_slot(slot)
        {
        }

        template <typename T>
        void operator() (T&& value) const
        {
            std::invoke(m_slot, m_receiver, voy_fwd(value));
        }

        void init()
        {
        }

        void notify_ended() const
        {
        }

    private:
        Object* m_receiver;
        Slot m_slot;
    };


} // namespace detail


template <typename T = void, typename Object = QObject, typename Signal = void>
auto signal(Object* sender, Signal&& signal)
{
    return detail::signal_impl<T, Object, Signal>(sender, voy_fwd(signal));
}


template <typename Object, typename Slot>
auto slot(Object* receiver, Slot&& slot)
{
    return detail::slot_impl<Object, Slot>(receiver, voy_fwd(slot));
}

} // namespace voy

#endif // include guard

