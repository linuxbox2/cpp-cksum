
cmake invocation:

cmake -G Ninja -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER='clang' -DCMAKE_CXX_COMPILER='clang++' -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_FLAGS="-O0 -g3 -gdwarf-4" -DCMAKE_C_FLAGS="-O0 -g3 -gdwarf-4" -DCMAKE_C_FLAGS_DEBUG="-O0 -g3 -gdwarf-4"  .
