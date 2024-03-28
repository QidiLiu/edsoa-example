#include "core/util/util.h"

extern const bool DEBUGGING_FLAG = true;

IWorker::IWorker(int init_worker_priority) {
    this->worker_default_priority = init_worker_priority;
}

IWorker::IWorker() {
    this->worker_default_priority = std::numeric_limits<int>::max();
}

void IWorker::send(const std::string& in_topic_name, const std::string& in_info) {
    if (this->message_queues.find(in_topic_name) != this->message_queues.end()) {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->message_queues[in_topic_name].push(Message{ this->worker_default_priority, in_info });
    } else {
        spdlog::warn("Topic not subsribed: {}.", in_topic_name);
    }
}

void IWorker::send(const std::string& in_topic_name, int in_priority, const std::string& in_info) {
    if (this->message_queues.find(in_topic_name) != this->message_queues.end()) {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->message_queues[in_topic_name].push(Message{ in_priority, in_info });
    } else {
        spdlog::warn("Topic not subsribed: {}.", in_topic_name);
    }
}

void IWorker::submitTask(const std::function<void()>& in_task) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->task_queue.push(in_task);
}


Secretary::Secretary() {
    this->main_loop_running_flag.store(false);
    this->thread_pool_ptr = std::make_shared<boost::asio::thread_pool>();
}

Secretary::~Secretary() {
    this->main_loop_running_flag.store(false);
    this->thread_pool_ptr->join();

    /*
    for (auto& topic : this->topics) {
        topic.second.cond_var.notify_all();
    }
    */
}

void Secretary::addTopic(const std::string& in_topic_name) {
    if (this->topics.find(in_topic_name) == this->topics.end()) {
        this->topics[in_topic_name];
        spdlog::info("Added topic: {}.", in_topic_name);
    } else {
        spdlog::warn("Topic already exists: {}.", in_topic_name);
    }
}

void Secretary::subscribe(std::shared_ptr<IWorker> init_worker_ptr, const std::string& init_topic_name) {
    this->topics[init_topic_name].subscribers.push_back(init_worker_ptr);
    std::lock_guard<std::mutex> lock(init_worker_ptr->mutex);

    if (init_worker_ptr->message_queues.find(init_topic_name) == init_worker_ptr->message_queues.end()) {
        init_worker_ptr->message_queues[init_topic_name];
        //spdlog::info("Subscribed worker: {} to topic: {}.", init_worker_ptr->getWorkerName(), init_topic_name);
        spdlog::info("Subscribed worker to topic: {}.", init_topic_name);
    } else {
        spdlog::warn("Worker already subscribed to topic: {}.", init_topic_name);
    }
}

void Secretary::startMainLoop() {
    this->main_loop_running_flag.store(true);
    spdlog::info("Started main loop.");

    while (this->main_loop_running_flag.load()) {
        for (auto& topic : this->topics) {

            this->gatherMessages(topic.first);

            if (!topic.second.messages.empty()) {
                this->distributeMessage(topic.second, topic.second.messages.top());
                topic.second.messages.pop();
            }

            this->gatherTasks(topic.first);

            if (!this->main_loop_running_flag.load()) { break; }
        }

        // execute tasks
        while (!this->task_queue.empty()) {
            boost::asio::post(*this->thread_pool_ptr, this->task_queue.front());
            this->task_queue.pop();
        }
    }
}

void Secretary::gatherMessages(const std::string& in_topic_name) {
    for (std::shared_ptr<IWorker> subscriber : this->topics[in_topic_name].subscribers) {
        while (!subscriber->message_queues[in_topic_name].empty()) {
            this->topics[in_topic_name].messages.push(subscriber->message_queues[in_topic_name].front());
            subscriber->message_queues[in_topic_name].pop();
        }
    }
}

void Secretary::distributeMessage(const Topic& in_topic, const Message& in_message) {
    for (std::shared_ptr<IWorker> subscriber : in_topic.subscribers) {
        subscriber->receive(in_message.info);
    }
}

void Secretary::gatherTasks(const std::string& in_topic_name) {
    for (std::shared_ptr<IWorker> subscriber : this->topics[in_topic_name].subscribers) {
        std::lock_guard<std::mutex> lock(subscriber->mutex);

        while (!subscriber->task_queue.empty()) {
            this->task_queue.push(subscriber->task_queue.front());
            subscriber->task_queue.pop();
        }
    }
}


