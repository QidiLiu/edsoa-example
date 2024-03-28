# edsoa-example

This is a simple implementation of "Event-driven SOA".

## Introduction

This project is an example project for the EDSOA framework. Please see [知乎 - Event-driven SOA](https://zhuanlan.zhihu.com/p/687635384) for more details. 

## Architecture

![Architecture](./other/img/Object_Model.excalidraw.png)

## Setup

To set up the project, follow these steps:

1. Create python virtual environment and activate it
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
