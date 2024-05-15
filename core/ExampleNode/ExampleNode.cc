#include "core/ExampleNode/ExampleNode.h"

ExampleNode::ExampleNode(
    int init_worker_priority, int example_init_delay) :
    IWorker(init_worker_priority) {
    this->example_var = 114514 + example_init_delay;
    this->example_delay = example_init_delay;
    this->example_data = nullptr;

    this->submitTask(std::bind(&ExampleNode::exampleInit, this));
}

//ExampleNode::~ExampleNode() {}

void ExampleNode::exampleInit() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(this->example_delay));

        if (this->example_delay == 1) {
            this->send("Topic_1", "testing sub-pub system (from node 1)");
        } else {
            this->send("Topic_1", "testing sub-pub system (from node 2)");
        }

        LOG(INFO) << "node_" << this->example_delay << " has sent command to Topic_1.";
    }
}

void ExampleNode::exampleFunc() {
    LOG(INFO) << "node_" << this->example_delay << ".exampleFunc() called.";
}

void ExampleNode::setData(const std::string& init_topic_name, IData* init_data_ptr) {
    if (init_topic_name == "example_data") {
        ExampleData* casted_ptr = dynamic_cast<ExampleData*>(init_data_ptr);

        if (casted_ptr == nullptr) {
            LOG(ERROR) << "Failed by casting data pointer";
        } else {
            this->example_data = casted_ptr;
        }
    }
}

void ExampleNode::receive(const std::string& in_info) {
    LOG(INFO) << "node_" << this->example_delay << " has received command on Topic_1. in_info: " << in_info;

    
    if (in_info.empty()) {
        LOG(ERROR) << "Received empty message";
        return;
    }

    if (in_info == "some other command") {
        // do something with method submitTask() like the code below
    }

    this->submitTask(std::bind(&ExampleNode::exampleFunc, this));
}

