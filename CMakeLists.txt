cmake_minimum_required(VERSION 3.0)
project(udp_pkt_loss_sim)

set(CMAKE_CXX_STANDARD 11)

# Add the executable target
add_executable(rtp_forwarder main.cpp)

# Link libraries if needed
# target_link_libraries(udp_pkt_loss_sim ...)

