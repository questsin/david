# Settings
project (chess_ann_src)

# Files to be compiled.
file(GLOB chess_ann_srcfiles
        main.cpp

        # utils namespace
        utils/utils.cpp
        utils/gameState.cpp
        utils/neuralNet.cpp

        # david namespace
        david/ChessEngine.cpp
        david/EngineMaster.cpp
        david/Search.cpp
        david/TreeGen.cpp
        david/MoveGen.cpp
        david/MoveGenTest.cpp
        ANN/ANN.cpp)
add_executable(chess_ann_src ${chess_ann_srcfiles})

target_compile_definitions(chess_ann_src PUBLIC DAVID_PRODUCTION)

# Make this folder linkable for parent folder.
target_link_libraries(chess_ann_src fann uci -lpthread)