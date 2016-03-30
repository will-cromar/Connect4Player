cmake_minimum_required(VERSION 3.3)
project(DoBetter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_executable(DoBetter con4lib.c con4lib.h connect4.c arupsoldplayer.c arupsoldplayer.h human_player.c human_player.h do_better.c do_better.h)
target_link_libraries(DoBetter m)