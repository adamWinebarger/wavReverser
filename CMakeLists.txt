cmake_minimum_required(VERSION 3.0)

project(wav_reverser LANGUAGES C)

add_executable(wav_reverser main.c)

install(TARGETS wav_reverser RUNTIME DESTINATION bin)
