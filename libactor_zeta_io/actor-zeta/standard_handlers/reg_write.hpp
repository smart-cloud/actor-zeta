#ifndef REG_WRITE_HPP
#define REG_WRITE_HPP

#include <map>
#include "actor-zeta/connection_handler.hpp"

namespace actor_zeta {
    namespace network {
        class reg_write final : public behavior::abstract_action {
        public:
            reg_write(std::map<std::string, write_handler> &actions) : actions(actions) {}

            void operator()(messaging::message &&msg) override {
                write_handler element = msg.get<write_handler>();
                actions.emplace(element.id(), std::move(element));
            }

            const std::string name() const override {
                return std::string("reg_write");
            };

        private:
            std::map<std::string, write_handler> &actions;
        };
    }
}
#endif //WRITE_HPP