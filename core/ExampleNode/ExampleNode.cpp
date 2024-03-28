#include "core/ExampleNode/ExampleNode.h"

ExampleNode::ExampleNode(
    int init_worker_priority, int example_init_delay) :
    ISimpleWorker(init_worker_priority) {
    this->example_var = 114514 + example_init_delay;
    this->example_delay = example_init_delay;

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

        spdlog::info("node_{} has sent command to Topic_1.", this->example_delay);
    }
}

void ExampleNode::exampleFunc() {
    spdlog::info("node_{}.exampleFunc() called.", this->example_delay);
}


void ExampleNode::receive(const std::string& in_info) {
    spdlog::info("node_{} has received command on Topic_1. in_info: {}", this->example_delay, in_info);

    
    if (in_info.empty()) {
        // message is empty, do nothing (or maybe throw an exception)
        return;
    }

    if (in_info == "some other command") {
        // do something with boost::asio::post() like the code below
    }

    this->submitTask(std::bind(&ExampleNode::exampleFunc, this));
}

