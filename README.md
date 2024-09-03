# HEMLOCK

A [SlackBuilds](https://slackbuilds.org) like package manager.

## Build

Build requires:
- CMake >= 3.5
- libsqlite3-dev

~~~
mkdir build
cd build
cmake .. -DSQLITE3_LIBRARY=<SHARED_LIB> -DSQLITE3_INCLUDE_DIRS=<INCLUDE_PATH>
cmake --build .
~~~

## License

[MIT License](/LICENSE)
Copyright (c) 2024 [The SoftFauna Team](/AUTHORS.md)