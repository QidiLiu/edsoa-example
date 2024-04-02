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
        std::lock_guard<std::mutex> lock(this->mtx);
        this->message_queues[in_topic_name].push(Message{ this->worker_default_priority, in_info });
    } else {
        spdlog::warn("Topic not subsribed: {}.", in_topic_name);
    }
}

void IWorker::send(const std::string& in_topic_name, int in_priority, const std::string& in_info) {
    if (this->message_queues.find(in_topic_name) != this->message_queues.end()) {
        std::lock_guard<std::mutex> lock(this->mtx);
        this->message_queues[in_topic_name].push(Message{ in_priority, in_info });
    } else {
        spdlog::warn("Topic not subsribed: {}.", in_topic_name);
    }
}

void IWorker::submitTask(const std::function<void()>& in_task) {
    std::lock_guard<std::mutex> lock(this->mtx);
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
    std::lock_guard<std::mutex> lock(init_worker_ptr->mtx);

    if (init_worker_ptr->message_queues.find(init_topic_name) == init_worker_ptr->message_queues.end()) {
        init_worker_ptr->message_queues[init_topic_name];
        //spdlog::info("Subscribed worker: {} to topic: {}.", init_worker_ptr->getWorkerName(), init_topic_name);
        spdlog::info("Subscribed worker to topic: {}.", init_topic_name);
    } else {
        spdlog::warn("Worker already subscribed to topic: {}.", init_topic_name);
    }
}

void Secretary::shareDataToTopic(const std::string& init_data_name, std::shared_ptr<IData> init_ptr, const std::string& init_topic_name) {
    if (this->topics.find(init_topic_name) == this->topics.end()) {
        spdlog::warn("Topic not found: {}.", init_topic_name);
    } else {
        this->topics[init_topic_name].data_ptrs[init_data_name] = init_ptr;
        spdlog::info("Shared data pointer: {} to topic: {}.", init_data_name, init_topic_name);
    }
}

std::shared_ptr<IData> Secretary::shareDataFromTopic(const std::string& in_data_name, const std::string& in_topic_name) {
    if (this->topics.find(in_topic_name) == this->topics.end()) {
        spdlog::warn("Topic not found: {}.", in_topic_name);
        return nullptr;
    } else if (this->topics[in_topic_name].data_ptrs.find(in_data_name) == this->topics[in_topic_name].data_ptrs.end()) {
        spdlog::warn("Data pointer not found in topic: {}.", in_topic_name);
        return nullptr;
    } else {
        spdlog::info("Shared data pointer: {} from topic: {}.", in_data_name, in_topic_name);
        return this->topics[in_topic_name].data_ptrs[in_data_name];
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
        std::lock_guard<std::mutex> lock(subscriber->mtx);

        while (!subscriber->task_queue.empty()) {
            this->task_queue.push(subscriber->task_queue.front());
            subscriber->task_queue.pop();
        }
    }
}

