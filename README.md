# edsoa-demo

This is a simple implementation of "Event-driven SOA".

## Introduction

This project is an example project for the EDSOA framework. Please see [知乎 - Event-driven SOA](https://zhuanlan.zhihu.com/p/687635384) for more details. 

## Setup

To set up the project, follow these steps:

1. Create python virtual environment and activate it (for Unix-like systems, use `source.venv/bin/activate` instead of `./.venv/Scripts/Activate.ps1`)
```bash
python -m venv .venv
./.venv/Scripts/Activate.ps1
python -m pip install --upgrade pip
pip install conan
```
2. Install dependencies with conan (if you don't have conan installed before, run `conan profile detect --force` first)
```bash
conan install . --build=missing
```
3. Install CMake extension for Vision Studio and then open the project folder in Visual Studio and it will set up the project for you automatically (if you're not a Visual Studio user, just consider this as a normal CMake project)

## TODO

- [ ] Initialize the thread num properly for thread pool
- [ ] Figure out wether an atomic bool is really needed to break out from while loop
- [ ] Simplify the data sharing mechanism with template
- [ ] Name two kinds of workers as "Worker" and "Pipeline", instead of "SimpleWorker" and "ComplexWorker"
- [ ] Implement "Pipeline", which is similar to VTK pipeline
- [ ] Implement a ROS2-Actor-like "Actor", which is parallel to the "Worker" and "Pipeline"
- [ ] Develop a design pattern for extending the framework
- [ ] Make some extensions for the framework, such as
  - [ ] "ImGui+VTK" bundle
  - [ ] gRPC-based middleware
  - [ ] TensorRT-based inference engine
  - [ ] RKNN-based inference engine
  - [ ] Bluetooth-based middleware
- [ ] Package the project as a library and publish it to conan-center
