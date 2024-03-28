#ifndef EXAMPLENODE_EXAMPLENODE_H_
#define EXAMPLENODE_EXAMPLENODE_H_

#include "core/util/util.h"

class ExampleNode : public ISimpleWorker {

public:

    ExampleNode(int init_worker_priority, int example_init_delay);
    //~ExampleNode();

    /// @brief [LOGIC] message processing
    /// @param in_info input message
    void receive(const std::string& in_info) override;

private:

    int example_var;
    int example_delay;

    /// @brief [LOGIC] example function
    void exampleInit();

    /// @brief [LOGIC] example function
    void exampleFunc();
};

#endif // EXAMPLENODE_EXAMPLENODE_H_
