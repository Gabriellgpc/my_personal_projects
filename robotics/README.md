# Sistemas Roboticos Autonomos
Projetos desenvolvidos durante o curso de Sistemas Robóticos Autônomos (Pós de elétrica e computação).

Arquitetura do Projeto:
- include
    - *.h/*.hpp

- lib
    - *.a
- program
  - p1m1
  - p2m2
  - p2m3
  - p2m1
  - p2m2
  - p2m3
  - p3m1
  - p3m2
  - p3m3
- source
  - b0RemoteApi
  - controllers
    - kinematicsControllers
    - pid
  - gnuplot-iostream (terceiros)
  - mapping
    - occupancy*_grid
  - matplotlib-cpp (terceiros)
  - planning
  - utils
    - configSpaceTools
    - mydatastructs
    - potentialField
    - utils (alguns estruturas, ferramentas matemáticas ...)
    

README.md

# Dependências

sudo apt-get install qt5-default

install g++:

sudo apt-get install build-essential

Install generic font configuration library - runtime

sudo apt-get install libfontconfig1

Install OpenGL libraries

sudo apt-get install mesa-common-dev

sudo apt-get install libglu1-mesa-dev -y


- ZeroMQ v4.1+
- Boost v1.54+
