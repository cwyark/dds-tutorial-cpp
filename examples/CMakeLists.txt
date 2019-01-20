cmake_minimum_required(VERSION 3.0)

find_package(OpenSplice REQUIRED)
find_package(log4cpp REQUIRED)

add_subdirectory(helloworld)
add_subdirectory(partition)
add_subdirectory(topickey)
add_subdirectory(contentfilter)
add_subdirectory(lifecycle)
add_subdirectory(ownership)
add_subdirectory(chatroom)
add_subdirectory(presentation)
add_subdirectory(coherent)
