#ifndef UTIL_UTIL_H_
#define UTIL_UTIL_H_

extern const bool DEBUGGING_FLAG;

#include <map>
#include <queue>
#include <limits>

#include <fmt/core.h>
#include "boost/asio.hpp"
#include "spdlog/spdlog.h"

class Message;


/// @brief base class for all workers
class IWorker {

public:

    /// @brief constructor
    /// @param init_worker_priority priority of the worker
    IWorker(int init_worker_priority);
    IWorker();

    virtual ~IWorker() {}

    virtual void receive(const std::string& in_info) = 0;

    // [topic_name: (msg_1, msg_2, ...);
    //  another_topic_name: (...);
    //  ...: ...]
    std::map<std::string, std::queue<Message>> message_queues;
    std::queue<std::function<void()>> task_queue;
    std::mutex mtx;

protected:

    int worker_default_priority;

    /// @brief send a message to a topic
    /// @param in_topic_name the topic to send to
    /// @param in_info the message content to send
    void send(const std::string& in_topic_name, const std::string& in_info);

    /// @brief send a message to a topic
    /// @param in_topic_name the topic to send to
    /// @param in_priority the priority of the message
    /// @param in_info the message content to send
    void send(const std::string& in_topic_name, int in_priority, const std::string& in_info);

    /// @brief submit a task to the task queue
    /// @param in_task the task to submit
    void submitTask(const std::function<void()>& in_task);

};


class ISimpleWorker : public IWorker {

public:

    ISimpleWorker(int init_worker_priority) : IWorker(init_worker_priority) {}
    ISimpleWorker() : IWorker() {}
    virtual ~ISimpleWorker() {}
};


class IComplexWorker : public IWorker {

public:

    IComplexWorker(int init_worker_priority) : IWorker(init_worker_priority) {}
    IComplexWorker() : IWorker() {}
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

struct IData {
    std::mutex mtx;
};

struct Topic {
    std::priority_queue<Message> messages;
    std::vector<std::shared_ptr<IWorker>> subscribers;
    std::map<std::string, std::shared_ptr<IData>> data_ptrs;
    //std::mutex mtx;
    //std::condition_variable cond_var;
};

class Secretary {

public:

    Secretary();
    ~Secretary();

    /// @brief add a new topic to the secretary
    /// @param in_topic_name the name of the new topic to add
    void addTopic(const std::string& in_topic_name);

    /// @brief subscribe a worker to a topic
    /// @param init_worker_ptr the worker to subscribe
    /// @param init_topic_name the name of the topic to subscribe to
    void subscribe(std::shared_ptr<IWorker> init_worker_ptr, const std::string& init_topic_name);

    /// @brief share data pointer to a topic
    /// @param init_data_name data's name
    /// @param init_ptr the data pointer to share
    /// @param init_topic_name the topic to share to
    void shareDataToTopic(const std::string& init_data_name, std::shared_ptr<IData> init_ptr, const std::string& init_topic_name);

    /// @brief share data pointer to a node
    /// @param in_data_name data's name
    /// @param in_topic_name data's topic
    /// @return the output data pointer
    std::shared_ptr<IData> shareDataFromTopic(const std::string& in_data_name, const std::string& in_topic_name);

    /// @brief start the main loop
    void startMainLoop();

    std::shared_ptr<boost::asio::thread_pool> thread_pool_ptr;

private:

    std::map<std::string, Topic> topics;
    std::atomic<bool> main_loop_running_flag;
    std::queue<std::function<void()>> task_queue;

    /// @brief gather messages to a topic
    /// @param in_topic_name the name of the topic to publish to
    void gatherMessages(const std::string& in_topic_name);

    /// @brief distribute the message with top priority to all subscribers of a topic
    /// @param in_topic for which topic to dispatch the command
    /// @param in_message the message to distribute
    void distributeMessage(const Topic& in_topic, const Message& in_message);


    void gatherTasks(const std::string& in_topic_name);
};

#endif // UTIL_UTIL_H_
