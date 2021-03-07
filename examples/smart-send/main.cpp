#include <cassert>

#include <unordered_set>
#include <queue>
#include <vector>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>

#include <actor-zeta.hpp>

using actor_zeta::basic_async_actor;
using actor_zeta::abstract_executor;
using actor_zeta::supervisor;
using actor_zeta::context;
using actor_zeta::join;

using actor_zeta::abstract_executor;
using actor_zeta::executor_t;
using actor_zeta::work_sharing;

using actor_zeta::make_message;


template<typename Task, typename ...Args>
inline auto make_task(supervisor&executor_,const std::string &command, Args... args ) -> void {
    executor_.enqueue(std::move(make_message(executor_.address(), command, std::move(Task(std::forward<Args>(args)...)))));
}

template<typename Task, typename ...Args>
inline auto make_task_broadcast(supervisor&executor_,const std::string &command, Args... args ) -> void {
    executor_.broadcast(make_message(executor_.address(), command, std::move(Task(std::forward<Args>(args)...))));
}


auto thread_pool_deleter = [](abstract_executor* ptr){
    ptr->stop();
    delete ptr;
};

/// non thread safe
class supervisor_lite final : public supervisor {
public:
    explicit supervisor_lite()
            : supervisor(actor_zeta::detail::string_view("network"))
            , e_(new executor_t<work_sharing>(
                    2,
                    100
                 ),
                 thread_pool_deleter
            )
            , cursor(0)
            , system_{"sync_contacts", "add_link", "remove_link"}
            {
    }

    ~supervisor_lite() override = default;

    auto shutdown() noexcept -> void {
        e_->stop();
    }

    auto startup() noexcept -> void {
        e_->start();
    }

    auto executor() noexcept -> actor_zeta::abstract_executor & final { return *e_; }

    auto join(actor_zeta::actor t) -> actor_zeta::actor_address final {
        auto tmp = std::move(t);
        auto address = tmp->address();
        actors_.push_back(std::move(tmp));
        return address;
    }

    auto enqueue(actor_zeta::message_ptr msg, actor_zeta::execution_device *) -> void final {
        auto msg_ = std::move(msg);
        auto it = system_.find(msg_->command());
        if (it != system_.end()) {
            local(std::move(msg_));
        } else {
            redirect_robin(std::move(msg_));
        }
    }

private:
    auto local(actor_zeta::message_ptr msg) -> void {
        set_current_message(std::move(msg));
        execute(*this);
    }

    auto redirect_robin(actor_zeta::message_ptr msg) -> void {
        if (!actors_.empty()) {
            actors_[cursor]->enqueue(std::move(msg));
            ++cursor;
            if (cursor >= actors_.size()) {
                cursor = 0;
            }
        }
        //assert(false);
    }

    std::unique_ptr<abstract_executor,decltype(thread_pool_deleter)> e_;
    std::vector<actor_zeta::actor> actors_;
    std::size_t cursor;
    std::unordered_set<actor_zeta::detail::string_view> system_;
};


struct download_data final {

    download_data(const std::string &url, const std::string &user, const std::string &passwod)
        : url_(url)
        , user_(user)
        , passwod_(passwod) {}

    ~download_data() = default;
    std::string url_;
    std::string user_;
    std::string passwod_;
};

struct work_data final {

    ~work_data() = default;

    work_data(const std::string &data, const std::string &operatorName) : data_(data), operator_name_(operatorName) {}

    std::string data_;
    std::string operator_name_;
};

static std::atomic<uint64_t> counter_download_data{0};
static std::atomic<uint64_t> counter_work_data{0};

class worker_t final : public basic_async_actor {
public:
    explicit worker_t(supervisor_lite &ref) : basic_async_actor(ref, "bot") {

        add_handler(
                "download",
                &worker_t::download
        );

        add_handler(
                "work_data",
                &worker_t::work_data
        );
    }

    void download(download_data &data){
        std::cerr << "url : " << data.url_ << std::endl;
        std::cerr << "user : " << data.user_ << std::endl;
        std::cerr << "password : " << data.passwod_ << std::endl;
        ++counter_download_data;
    }

    void work_data(work_data & data) {

        std::cerr << "data_ : " << data.data_ << std::endl;
        std::cerr << "operator_name : " << data.operator_name_ << std::endl;
        ++counter_work_data;
    }

    ~worker_t() override = default;

private:

};

using namespace std::chrono_literals;

int main() {

    actor_zeta::intrusive_ptr<supervisor_lite> supervisor(new supervisor_lite());

    supervisor->startup();

    int const actors = 10;

    for (auto i = actors - 1; i > 0; --i) {
        auto bot = join<worker_t>(*supervisor);
        actor_zeta::link(*supervisor, bot);
    }

    int const task = 10000;

    for (auto i = task - 1; i > 0; --i) {
        make_task<download_data>(*supervisor,"download", "fb", "jack","1");
    }

    for (auto i = task - 1; i > 0; --i) {
        make_task_broadcast<work_data>(*supervisor,"work_data", "fb", "jack");
    }

    for(;counter_work_data == task &&counter_download_data == task ;) {
        std::this_thread::sleep_for(250ms);
        std::cerr << counter_work_data << std::endl;
        std::this_thread::sleep_for(250ms);
    }

    std::this_thread::sleep_for(60s);
    return 0;
}