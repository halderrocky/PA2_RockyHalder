cmake_minimum_required(VERSION 4.3)
project(PA2_RockyHalder)

set(CMAKE_CXX_STANDARD 20)

add_executable(PA2_RockyHalder main.cpp
        StoryTree.h
        Item.h
        Adventure.h
        Scene.h
        Node.h
        Moment.h
        List.h
        Adventure.cpp)
