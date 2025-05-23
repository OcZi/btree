message("Compiling BTree of Algorithm and Data structures ...")

cmake_minimum_required(VERSION 3.30.5)

project(BTREE_AED DESCRIPTION "A repository of Algorithms and Data structures course in UTEC" LANGUAGES CXX)

SET(SRC ${PROJECT_SOURCE_DIR}/src)

add_executable(
        ${PROJECT_NAME}
        btree.h
        node.h
        main.cpp
        tester.h
        btree_train.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/inc
)