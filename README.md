# MAS-Platform-C

![](https://img.shields.io/badge/lang-C-green)
![](https://img.shields.io/badge/category-Artificial%20Intelligence-blue)

- [Introduction](#introduction)
- [Demo: Building & Running](#demo--building---running)
  * [Linux](#linux)

## Introduction

Project developed by a Scientific Initiation team with a scholarship from [SUPER](https://super.ufam.edu.br/), a partnership between Samsung and the [Federal University of Amazonas (UFAM)](https://ufam.edu.br/) that aims to stimulate training and research.

This project is a **PoC** to research the viability of making an Intelligent [Multi-Agent System (**MAS**)](https://en.wikipedia.org/wiki/Multi-agent_system) in C Language.

The project code implements a **MAS** development platform. Through the platform it is possible to develop customized MAS systems, control their behavior, interactions and way of execution.

In this **PoC**, a **MAS** demo was developed to test the capabilities of the platform. This demo can be built and run. For more information see the section: [Demo: Building & Running](#demo--building---running)

During the tests, the platform proved to be stable. Even with several agents running at the same time and interacting with each other, the program remained stable and produced the expected results. Further improvements and tests are required.

## Demo: Building & Running

### Linux

To build the project, make sure you have `gcc` and `make` installed. To check, you can run the commands:

```console
gcc --version
make --version
```

If the version of `gcc` and `make` appears, you are good to go. Otherwise, you can install them via apt:

```console
sudo apt update
sudo apt install build-essential
```

With everything ready, it's time to build. To do so, run `make` in the project root directory:

```console
make
```

The binary `cmas` will be created, and can be executed by running:

```console
./cmas
```