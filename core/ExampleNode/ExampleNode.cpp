#include "core/util/util.h"
#include "core/ExampleNode/ExampleNode.h"

ExampleNode::ExampleNode(
    std::shared_ptr<Secretary> init_secretary, int init_worker_priority,
    int example_init_delay) :
    ISimpleWorker(init_secretary, init_worker_priority) {
    this->example_var = 114514 + example_init_delay;
    this->example_delay = example_init_delay;
}

//ExampleNode::~ExampleNode() {}

void ExampleNode::exampleFunc() {
    spdlog::info("node_{}.exampleFunc() called.", this->example_delay);
}

void ExampleNode::run() {

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

void ExampleNode::receive(const std::string& in_info) {
    spdlog::info("node_{} has received command on Topic_1. in_info: {}", this->example_delay, in_info);

    
    if (in_info.empty()) {
        // message is empty, do nothing (or maybe throw an exception)
        return;
    }

    if (in_info == "some other command") {
        // do something with boost::asio::post() like the code below
    }

    boost::asio::post(*this->secretary->thread_pool_ptr, std::bind(&ExampleNode::exampleFunc, this));
}

