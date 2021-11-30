cmake_minimum_required(VERSION 3.17)
project(archivr C)

set(CMAKE_C_STANDARD 99)

add_executable(archivr main.c util.h util.c ARCH.h ARCH.c unordered_map.c unordered_map.h node.c node.h NIT.c NIT.h IT.c IT.h info.c info.h dirmapper.c dirmapper.h crypto.c crypto.h )