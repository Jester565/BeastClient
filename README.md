# :japanese_ogre:BeastClient
The boost asio and beast examples makes using their libraries seem excessively difficult. BeastClient is a good example of utilizing
the two libraries to create an HTTPSClient.  You can see the Service class for an example of using the client to communicate with [Consul](https://www.consul.io/). 

## Installation
Requirements: [CMake](https://cmake.org/), [Boost](https://www.boost.org/) Threading & Regex
```
mkdir build
cd build
cmake ..
```
On windows, build using the sln file.  On unix, run make.

### Troubleshooting
If cmake can't find Boost you can specify your boost location in the cmake command like so:
```
cmake -DBoost_INCLUDE_DIRS="/my/boost_1_66_0" -DBoost_LIBRARY_DIRS="/my/boost_1_66_0/stage/lib" ../
```

On windows, this project expects a static boost libraries with MT linking.
