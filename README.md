# REST API for users impemented on Crow with C++    

Example of REST API implemented on C++ with Crow framework.  

## Instalation  

Requirements:   
- boost  

Installation steps:  
- `git clone git@github.com:CrowCpp/Crow.git crow`   
- `cd crow && mkdir build && cd build`  
- `cmake .. && make`   
- `mv crow_all.h ../crow.hpp && cd ..`  
- `gcc users_rest.cpp crow.hpp -lstdc++ -lboost_system -pthread -o users_rest`
- `./users_rest 5000`  



