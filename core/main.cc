#include "util/util.h"
#include "util/data/ExampleData.hpp"
#include "core/ExampleNode/ExampleNode.h"

int main() {
    std::shared_ptr<Secretary> secretary = std::make_shared<Secretary>(6);
    auto example_data = absl::make_unique<ExampleData>();
    secretary->addTopic("example_data", example_data.get());
    secretary->addTopic("Topic_1");
    secretary->addTopic("Topic_2");

    std::shared_ptr<ExampleNode> node_1 = std::make_shared<ExampleNode>(0, 1);
    secretary->subscribe(node_1, "example_data");
    secretary->subscribe(node_1, "Topic_1");

    std::shared_ptr<ExampleNode> node_2 = std::make_shared<ExampleNode>(1, 2);
    secretary->subscribe(node_2, "Topic_1");
    secretary->subscribe(node_2, "Topic_2");

    secretary->startMainLoop();

    return 0;
}
