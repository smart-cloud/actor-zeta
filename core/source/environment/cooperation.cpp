#include "actor/behavior.hpp"
#include "environment/cooperation.hpp"
#include "environment/group.hpp"
#include "actor/local_actor.hpp"
#include <algorithm>

namespace actor_zeta {

    void cooperation::async_send(messaging::message &&msg) {
        cooperation_groups[entry_point].async_send(std::move(msg));
    }

    void cooperation::sync(std::initializer_list<std::string> list_names) {
        entry_point = *(list_names.begin());
        std::vector<std::string> tmp(list_names);
        std::reverse(tmp.begin(), tmp.end());

        for (auto i = tmp.begin(); i != tmp.end(); ++i) {
            auto j = i;
            ++j;
            if (j == tmp.end()) {
                return;
            }
            cooperation_groups[*(j)].async_send(
                    messaging::make_message(
                            std::string("sync_contacts"),
                            cooperation_groups[*(i)].address_entry_point()
                    )
            );
        }
    }

    void cooperation::sync() {
        if (cooperation_groups.size() == 1) {
            entry_point = cooperation_groups.begin()->first;
        }
    }

    cooperation &cooperation::add(group &&g) {
        cooperation_groups.emplace(g.name_entry_point(), std::move(g));
        return *this;
    }

    void cooperation::async_send_all(messaging::message &&doc) {
        for (auto &i:cooperation_groups) {
            i.second.async_send_all(std::move(doc));
        }
    }

    cooperation &cooperation::set_exeutor(const std::string &name_group, abstract_coordinator_ptr e) {
        cooperation_groups[name_group].set_exeutor_all(e);
        return *this;
    }

    cooperation &cooperation::set_exeutor_all(abstract_coordinator_ptr e) {
        for (auto &i:cooperation_groups) {
            i.second.set_exeutor_all(e);
        }
        return *this;
    }

    cooperation &cooperation::add_sharet_address(actor_address address) {
        for (auto &i:cooperation_groups) {
            i.second.add_shared_address(address);
        }
        return *this;
    }

    void cooperation::async_send(const std::string &name, messaging::message &&message) {
        cooperation_groups[name].async_send(std::move(message));

    }
}