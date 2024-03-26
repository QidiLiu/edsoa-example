#include "spdlog/spdlog.h"

#include "core/util/util.h"
#include "core/ExampleNode/ExampleNode.h"

int main() {
    std::shared_ptr<Secretary> secretary = std::make_shared<Secretary>();
    secretary->addTopic("Topic_1");
    secretary->addTopic("Topic_2");

    std::shared_ptr<ExampleNode> node_1 = std::make_shared<ExampleNode>(secretary, 0, 1);
    node_1->subscribe("Topic_1");
    boost::asio::post(*secretary->thread_pool_ptr, std::bind(&ExampleNode::run, node_1));

    std::shared_ptr<ExampleNode> node_2 = std::make_shared<ExampleNode>(secretary, 1, 2);
    node_2->subscribe("Topic_1");
    node_2->subscribe("Topic_2");
    boost::asio::post(*secretary->thread_pool_ptr, std::bind(&ExampleNode::run, node_2));

    secretary->startMainLoop();

    return 0;
}
