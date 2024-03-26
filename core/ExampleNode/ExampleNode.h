#ifndef EXAMPLENODE_EXAMPLENODE_H_
#define EXAMPLENODE_EXAMPLENODE_H_

#include <fmt/core.h>
#include "spdlog/spdlog.h"

class ExampleNode : public ISimpleWorker {

public:

    ExampleNode(std::shared_ptr<Secretary> init_decision_maker_ptr, int init_worker_priority, int example_init_delay);
    //~ExampleNode();

    /// @brief [LOGIC] optional function to activates the worker
    void run() override;
   
    /// @brief [LOGIC] message processing
    /// @param in_info input message
    void receive(const std::string& in_info) override;

private:

    int example_var;
    int example_delay;

    /// @brief [LOGIC] example function
    void exampleFunc();
};

#endif // EXAMPLENODE_EXAMPLENODE_H_
