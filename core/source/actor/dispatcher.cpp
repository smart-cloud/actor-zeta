#include <actor-zeta/base/dispatcher.hpp>

#include <iostream>
#include <memory>

// clang-format off
#include <actor-zeta/base/context.hpp>
#include <actor-zeta/base/actor_address.hpp>
#include <actor-zeta/messaging/message.hpp>
#include <actor-zeta/base/handler.hpp>
// clang-format on

namespace actor_zeta { namespace base {

        inline void error_duplicate_handler(detail::string_view _error_ ) {
            std::cerr << "Duplicate" << std::endl;
            std::cerr << "Handler: " << _error_ << std::endl;
            std::cerr << "Duplicate" << std::endl;
        }

        inline void error_add_handler(detail::string_view _error_ ) {
            std::cerr << "error add handler" << std::endl;
            std::cerr << "Handler: " << _error_ << std::endl;
            std::cerr << "error add handler" << std::endl;
        }

        inline void error_skip(detail::string_view  __error__) {
            std::cerr << "WARNING" << std::endl;
            std::cerr << "Skip : " << __error__ << std::endl;
            std::cerr << "WARNING" << std::endl;
        }

        void dispatcher_t::execute(context &ctx) {
            auto it = handlers_.find(ctx.current_message().command());
            if (it != handlers_.end()) {
                return it->second->invoke(ctx);
            } else {
                error_skip(ctx.current_message().command());
            }
        }

        bool dispatcher_t::on(detail::string_view name, handler *aa) {
            auto it = handlers_.find(name);
            bool status = false ;
            if( it == handlers_.end() ) {
                auto it1 =  handlers_.emplace(name, std::unique_ptr<handler>(aa));
                status = it1.second;
                if(status == false){
                    error_add_handler(name);
                }
            } else {
                error_duplicate_handler(name);
            }

            return status;
        }

        auto dispatcher_t::end() -> iterator {
            return handlers_.end();
        }

        auto dispatcher_t::begin() -> iterator {
            return handlers_.begin();
        }

        auto dispatcher_t::cend() -> const_iterator {
            return handlers_.cend();
        }

        auto dispatcher_t::cbegin() -> const_iterator {
            return handlers_.cbegin();
        }

        auto dispatcher_t::begin() const -> const_iterator {
            return handlers_.begin();
        }

        auto dispatcher_t::end() const -> const_iterator {
            return handlers_.end();
        }

}}
