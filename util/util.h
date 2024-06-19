#ifndef UTIL_UTIL_H_
#define UTIL_UTIL_H_

#include <queue>
#include <limits>

#include <absl/log/log.h>
#include <absl/synchronization/mutex.h>
#include <absl/synchronization/internal/thread_pool.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/inlined_vector.h>
#include <ini.h>
#include "INIReader.h"

extern const INIReader INI_READER;
extern const bool DEBUGGING_MODE;

class Message;
struct IData;


/// @brief base class for all workers
class IWorker {

public:

    /// @brief constructor
    /// @param init_worker_priority priority of the worker
    IWorker(int init_worker_priority);
    IWorker();

    virtual ~IWorker() = default;

    virtual void setData(const std::string& init_topic_name, IData* init_data_ptr) = 0;

    virtual void receive(const std::string& in_info) = 0;

    //// [topic_name: (msg_1, msg_2, ...);
    ////  another_topic_name: (...);
    ////  ...: ...]
    //std::map<std::string, std::queue<Message>> message_queues;
    absl::flat_hash_map<std::string, std::queue<Message>> message_queues;
    std::queue<std::function<void()>> task_queue;
    //std::mutex mtx;
    absl::Mutex mtx;

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

    // TODO broadcast message to all topics: void broadcast(const std::string& in_info);

    /// @brief submit a task to the task queue
    /// @param in_task the task to submit
    void submitTask(std::function<void()> in_task);

};


struct Message {
    int priority;
    std::string info;

    bool operator<(const Message& other) const {
        return this->priority < other.priority;
    }
};

struct IData {
    mutable absl::Mutex mtx;

    virtual ~IData() {}
};

struct Topic {
    std::priority_queue<Message> messages;
    std::vector<std::shared_ptr<IWorker>> subscribers;
    IData* data;

    Topic() : data(nullptr) {}
    Topic(IData* init_data_ptr) : data(init_data_ptr) {}
};

class Secretary {

public:

    Secretary(int init_thread_num);
    ~Secretary();

    /// @brief add a new topic to the secretary
    /// @param in_topic_name the name of the new topic to add
    void addTopic(const std::string& in_topic_name, IData* in_data_ptr = nullptr);

    /// @brief subscribe a worker to a topic
    /// @param init_worker_ptr the worker to subscribe
    /// @param init_topic_name the name of the topic to subscribe to
    void subscribe(std::shared_ptr<IWorker> init_worker_ptr, const std::string& init_topic_name);

    /// @brief start the main loop
    void startMainLoop();

    std::shared_ptr<absl::synchronization_internal::ThreadPool> thread_pool;

private:

    absl::flat_hash_map<std::string, Topic> topics;
    std::atomic<bool> main_loop_running_flag;
    std::queue<std::function<void()>> task_queue;

    /// @brief gather messages to a topic
    /// @param in_topic_name the name of the topic to publish to
    void gatherMessages(const std::string& in_topic_name);

    /// @brief distribute the message with top priority to all subscribers of a topic
    /// @param in_topic for which topic to dispatch the command
    /// @param in_message the message to distribute
    void distributeMessage(const Topic& in_topic, const Message& in_message);

    void broadcastStopMessage();

    void gatherTasks(const std::string& in_topic_name);
};

#endif // UTIL_UTIL_H_
