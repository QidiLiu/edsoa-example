#include "core/util/util.h"

extern const bool DEBUGGING_FLAG = true;

IWorker::IWorker(std::shared_ptr<Secretary> init_secretary, int init_worker_priority) {
    this->secretary = init_secretary;
    this->worker_priority = init_worker_priority;
}

void IWorker::subscribe(const std::string& in_topic_name) {
    this->secretary->subscribe(in_topic_name, this);
}

void IWorker::send(const std::string& in_topic_name, const std::string& in_info) {
    this->secretary->publish(in_topic_name, this->worker_priority, in_info);
}


Secretary::Secretary() {
    this->main_loop_running_flag.store(false);
    this->thread_pool_ptr = std::make_shared<boost::asio::thread_pool>();
}

Secretary::~Secretary() {
    this->main_loop_running_flag.store(false);
    this->thread_pool_ptr->join();

    for (auto& pair : this->topics) {
        pair.second.cond_var.notify_all();
    }
}

void Secretary::addTopic(const std::string& in_topic_name) {
    this->topics[in_topic_name];
}

void Secretary::subscribe(const std::string& init_topic_name, IWorker* init_worker_ptr) {
    this->topics[init_topic_name].subscribers.push_back(init_worker_ptr);
}

void Secretary::startMainLoop() {
    this->main_loop_running_flag.store(true);

    while (this->main_loop_running_flag.load()) {
        for (auto& topic : this->topics) {
            std::lock_guard<std::mutex> lock(topic.second.mutex);

            if (!topic.second.messages.empty()) {
                Message message = topic.second.messages.top();
                topic.second.messages.pop();
                this->dispatchCommand(topic.second, message);
            }

            if (!this->main_loop_running_flag.load()) { break; }
        }
    }
}

void Secretary::publish(const std::string& in_topic_name, int in_priority, const std::string& in_info) {
    std::lock_guard<std::mutex> lock(this->topics[in_topic_name].mutex);
    this->topics[in_topic_name].messages.push(Message{ in_priority, in_info });
}

void Secretary::dispatchCommand(const Topic& in_topic, const Message& in_message) {
    for (IWorker* subscriber : in_topic.subscribers) {
        subscriber->receive(in_message.info);
    }
}

/*
PYBIND11_MODULE(Secretary, m) {
    m.doc() = "Interface of modules for Python. Developed by QidiLiu.";

    py::class_<Secretary>(m, "Secretary", py::dynamic_attr())
        .def(py::init<>())
        .def("addTopic", &Secretary::addTopic)
        .def("startMainLoop", &Secretary::startMainLoop);
}
*/


