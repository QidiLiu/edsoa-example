#include "util/util.h"

extern const INIReader INI_READER("../../meta/config/config.ini");
extern const bool DEBUGGING_MODE = INI_READER.GetBoolean("DEFAULT", "DEBUGGING_MODE", false);

IWorker::IWorker(int init_worker_priority) {
    this->worker_default_priority = init_worker_priority;
}

IWorker::IWorker() {
    this->worker_default_priority = std::numeric_limits<int>::max();
}

void IWorker::send(const std::string& in_topic_name, const std::string& in_info) {
    absl::MutexLock lock(&this->mtx);
    if (this->message_queues.contains(in_topic_name)) {
        this->message_queues[in_topic_name].push(Message{ this->worker_default_priority, in_info });
    } else {
        LOG(WARNING) << "Topic not subscribed: " << in_topic_name;
    }
}

void IWorker::send(const std::string& in_topic_name, int in_priority, const std::string& in_info) {
    absl::MutexLock lock(&this->mtx);
    if (this->message_queues.contains(in_topic_name)) {
        this->message_queues[in_topic_name].push(Message{ in_priority, in_info });
    } else {
        LOG(WARNING) << "Topic not subscribed: " << in_topic_name;
    }
}

void IWorker::submitTask(std::function<void()> in_task) {
    absl::MutexLock lock(&mtx);
    this->task_queue.push(std::move(in_task));
}


Secretary::Secretary(int init_thread_num) {
    if (INI_READER.ParseError() < 0) {
        LOG(ERROR) << "Failed to parse config.ini";
    }

    if (DEBUGGING_MODE) {
        LOG(INFO) << "Debugging mode is on.";
    }

    this->main_loop_running_flag.store(false);
    this->thread_pool = std::make_shared<absl::synchronization_internal::ThreadPool>(init_thread_num);
    this->addTopic("COMMON");
}

Secretary::~Secretary() {
    this->main_loop_running_flag.store(false);
}

void Secretary::addTopic(const std::string& in_topic_name, IData* in_data_ptr) {
    if (this->topics.contains(in_topic_name)) {
        LOG(WARNING) << "Topic already exists: " << in_topic_name;
    } else {
        this->topics[in_topic_name] = Topic(in_data_ptr);
        LOG(INFO) << "Added topic: " << in_topic_name;
    }
}

void Secretary::subscribe(std::shared_ptr<IWorker> init_worker_ptr, const std::string& init_topic_name) {
    this->topics[init_topic_name].subscribers.push_back(init_worker_ptr);
    absl::MutexLock lock(&init_worker_ptr->mtx);

    if (!init_worker_ptr->message_queues.contains("COMMON")) {
        init_worker_ptr->message_queues["COMMON"] = std::queue<Message>();
        this->topics["COMMON"].subscribers.push_back(init_worker_ptr);
        LOG(INFO) << "Subscribed worker to topic: COMMON";
    }

    if (init_worker_ptr->message_queues.contains(init_topic_name)) {
        LOG(WARNING) << "Worker already subscribed to topic: " << init_topic_name;
    } else {
        init_worker_ptr->message_queues[init_topic_name] = std::queue<Message>();
        init_worker_ptr->setData(init_topic_name, this->topics[init_topic_name].data);
        LOG(INFO) << "Subscribed worker to topic: " << init_topic_name;
    }
}

void Secretary::startMainLoop() {
    this->main_loop_running_flag.store(true);
    LOG(INFO) << "Started main loop.";

    while (this->main_loop_running_flag.load()) {
        for (auto& topic_pair : this->topics) {
            auto& topic = topic_pair.second;
            const std::string& topic_name = topic_pair.first;

            this->gatherMessages(topic_name);

            if (!topic.messages.empty()) {
                this->distributeMessage(topic, topic.messages.top());

                if (topic.messages.top().info == "STOP") {
                    this->main_loop_running_flag.store(false);
                    this->broadcastStopMessage();
                    topic.messages.pop();
                    break;
                }

                topic.messages.pop();
            }

            this->gatherTasks(topic_name);
        }

        if (!this->main_loop_running_flag.load()) { break; }

        // execute tasks
        while (!this->task_queue.empty()) {
            this->thread_pool->Schedule(this->task_queue.front());
            this->task_queue.pop();
        }
    }
}

void Secretary::gatherMessages(const std::string& in_topic_name) {
    for (std::shared_ptr<IWorker> subscriber : this->topics[in_topic_name].subscribers) {
        while (!subscriber->message_queues[in_topic_name].empty()) {
            Message msg = subscriber->message_queues[in_topic_name].front();
            this->topics[in_topic_name].messages.push(std::move(msg));
            subscriber->message_queues[in_topic_name].pop();
        }
    }
}

void Secretary::distributeMessage(const Topic& in_topic, const Message& in_message) {
    for (std::shared_ptr<IWorker> subscriber : in_topic.subscribers) {
        subscriber->receive(in_message.info);
    }
}

void Secretary::broadcastStopMessage() {
    for (auto& topic_pair : this->topics) {
        for (std::shared_ptr<IWorker> subscriber : topic_pair.second.subscribers) {
            subscriber->receive("STOP");
        }
    }
}

void Secretary::gatherTasks(const std::string& in_topic_name) {
    for (std::shared_ptr<IWorker> subscriber : this->topics[in_topic_name].subscribers) {
        absl::MutexLock lock(&subscriber->mtx);
        while (!subscriber->task_queue.empty()) {
            this->task_queue.push(std::move(subscriber->task_queue.front()));
            subscriber->task_queue.pop();
        }
    }
}

