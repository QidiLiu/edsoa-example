#ifndef UTIL_UTIL_H_
#define UTIL_UTIL_H_

extern const bool DEBUGGING_FLAG;

#include <queue>

//#include <pybind11/pybind11.h>
//namespace py = pybind11;

#include <fmt/core.h>
#include "boost/asio.hpp"
#include "spdlog/spdlog.h"

class Secretary;


/// @brief base class for all workers
class IWorker {

public:

    /// @brief constructor
    /// @param init_secretary bridge to the topic manager
    /// @param init_worker_priority priority of the worker
    IWorker(std::shared_ptr<Secretary> init_secretary, int init_worker_priority);

    virtual ~IWorker() {}

    /// @brief subscribe to a topic
    /// @param in_topic_name the topic to subscribe to
    void subscribe(const std::string& in_topic_name);

    virtual void receive(const std::string& in_info) = 0;

    /// @brief send a message to a topic
    /// @param in_topic_name the topic to send to
    /// @param in_info the message content to send
    void send(const std::string& in_topic_name, const std::string& in_info);

protected:

    std::shared_ptr<Secretary> secretary;
    int worker_priority;
};


class ISimpleWorker : public IWorker {

public:

    ISimpleWorker(std::shared_ptr<Secretary> init_secretary, int init_worker_priority) : IWorker(init_secretary, init_worker_priority) {}
    virtual ~ISimpleWorker() {}

    virtual void run() = 0;
};


class IComplexWorker : public IWorker {

public:

    IComplexWorker(std::shared_ptr<Secretary> init_secretary, int init_worker_priority) : IWorker(init_secretary, init_worker_priority) {}
    virtual ~IComplexWorker() {}

    virtual void start() = 0;
};


struct Message {
    int priority;
    std::string info;

    bool operator<(const Message& other) const {
        return this->priority < other.priority;
    }
};

struct Topic {
    std::priority_queue<Message> messages;
    std::vector<IWorker*> subscribers;
    std::mutex mutex;
    std::condition_variable cond_var;
};

class Secretary {

public:

    Secretary();
    ~Secretary();

    /// @brief add a new topic to the secretary
    /// @param in_topic_name the name of the new topic to add
    void addTopic(const std::string& in_topic_name);

    /// @brief subscribe a worker to a topic
    /// @param init_topic_name the name of the topic to subscribe to
    /// @param init_worker_ptr the worker to subscribe
    void subscribe(const std::string& init_topic_name, IWorker* init_worker_ptr);

    /// @brief start the main loop
    void startMainLoop();

    /// @brief publish a message to a topic
    /// @param in_topic_name the name of the topic to publish to
    /// @param in_priority the priority of the message
    /// @param in_info the message content to publish
    void publish(const std::string& in_topic_name, int in_priority, const std::string& in_info);

    std::shared_ptr<boost::asio::thread_pool> thread_pool_ptr;

private:

    std::unordered_map<std::string, Topic> topics;
    std::atomic<bool> main_loop_running_flag;

    /// @brief dispatch a command to all subscribers of a topic
    /// @param in_topic for which topic to dispatch the command
    /// @param in_message the message to dispatch
    void dispatchCommand(const Topic& in_topic, const Message& in_message);
};

#endif // UTIL_UTIL_H_
