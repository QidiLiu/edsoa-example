# edsoa-example

This is an example project for the EDSOA framework.

## Introduction

This project is an example project for the EDSOA framework. It demonstrates how to use the framework to create a microservice application. 

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
3. Open the project folder in Visual Studio and it will set up the project for you automatically
