# Advanced Computing Challenge

## General instructions

* The solution should be inside TradingAggregator and TradingBroadcaster folders
* You can update CMakeLists.txt to add new source files
* A sample of the API to store the latest prices is available on StorageApi folder. In this folder you can see all endpoints provided by the Storage API. There is no need to change anything in that folder.
* A sample of the DataProvider is available on DataProvider folder. You can use that in your tests

## Build the solution
You can build the project in pretty much the same way as any other CMake-based project. 
As one of the simplest methods, you can build the project from the command line by running the following commands in the current directory:

```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Run solution locally
To run the solution locally you can run each executable separately from command line, or you can create script to run all of them with single command.
You can use command line arguments when running multiple instances of TradingAggregator and DataProvider.