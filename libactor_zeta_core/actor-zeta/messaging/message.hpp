#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <memory>

#include "actor-zeta/actor/actor_address.hpp"
#include "message_priorities.hpp"
#include "actor-zeta/any.hpp"

namespace actor_zeta {
    namespace messaging {
        class message {
        public:
            message() = default;

            message(const message &) = default;

            message &operator=(const message &) = default;

            ~message() {
            }

            template<typename T>
            message(const std::string &type, const T &t)
                    : type_(type), content(t), prioritie(message_priorities::normal), callback(false) {}

            template<typename T>
            message(const std::string &type, const T &t, const actor::actor_address &aa)
                    : type_(type), content(t), prioritie(message_priorities::normal), callback(false), address(aa) {}

            template<typename T>
            message(const std::string &type, const T &t, message_priorities p)
                    : type_(type), content(t), prioritie(p), callback(false) {}

            template<typename T>
            message(const std::string &type, const T &t, message_priorities p, const actor::actor_address &aa)
                    : type_(type), content(t), prioritie(p), callback(true), address(aa) {}

            const message_priorities get_priorities() const {
                return prioritie;
            }

            const std::string &type() const {
                return type_;
            }

            const actor::actor_address &return_address() const {
                return address;
            };

            const bool is_callback() const {
                return callback;
            }

            template <typename  T>
            T get() {
                return any_cast<T>(content);
            }

            message *copy() {
                message* tmp =new message;
                tmp->content=this->content;
                tmp->callback=this->callback;
                tmp->type_=this->type_;
                tmp->address=this->address;
                tmp->prioritie=this->prioritie;
                return tmp;
            }

        private:
            any content;
            bool callback;
            std::string type_;
            actor::actor_address address;
            message_priorities prioritie;

        };

        template<typename T>
        inline message* make_message(const std::string &command, T data) {
            return new message(command, data);
        }

        template<typename T>
        inline message* make_message(const std::string &command, T data, const actor::actor_address &address) {
            return new message(command, data, address);
        }
    }
}
#endif