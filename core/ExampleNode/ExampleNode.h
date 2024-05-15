#ifndef EXAMPLENODE_EXAMPLENODE_H_
#define EXAMPLENODE_EXAMPLENODE_H_

#include "util/util.h"
#include "util/data/ExampleData.hpp"

class ExampleNode : public IWorker {

public:

    ExampleNode(int init_worker_priority, int example_init_delay);
    //~ExampleNode();

    /// @brief [LOGIC] set custom data for your node
    /// @param init_topic_name data source topic name
    /// @param init_data_ptr data pointer
    void setData(const std::string& init_topic_name, IData* init_data_ptr) override;

    /// @brief [LOGIC] message processing
    /// @param in_info input message
    void receive(const std::string& in_info) override;

private:

    ExampleData* example_data;
    int example_var;
    int example_delay;

    /// @brief [LOGIC] example function
    void exampleInit();

    /// @brief [LOGIC] example function
    void exampleFunc();
};

#endif // EXAMPLENODE_EXAMPLENODE_H_
