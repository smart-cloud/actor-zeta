#ifndef GROUP_HPP
#define GROUP_HPP

#include <map>
#include <string>
#include "messaging/message.hpp"
#include "fwd.hpp"

namespace actor_zeta {
    class group {
    public:
        group() = default;

        group(const group &a) = delete;

        group(group &&) = default;

        group &operator=(const group &a) = delete;

        group &operator=(group &&) = default;

        group(abstract_actor *);

        ~group() { };

        std::string name_entry_point() const;

        group &add(actor &&);

        group &add(abstract_actor *);

        group &add(const std::string &, abstract_actor *);

        group &add_shared_address(actor_address);

        void sync(std::initializer_list<std::string>);

        void sync();

        actor_address address_entry_point();

        void async_send(messaging::message &&);

        void async_send_all(messaging::message &&);

        group &set_exeutor(const std::string &, abstract_coordinator_ptr);

        group &set_exeutor_all(abstract_coordinator_ptr);

    private:
        std::map<std::string, actor> unique_actors;
        std::string entry_point;
    };


    template<class V>
    inline void send(actor_zeta::group &g, std::string commanda, V value) {
        g.async_send(std::move(messaging::make_message(commanda, value)));
    }
}
#endif //GROUP_HPP
