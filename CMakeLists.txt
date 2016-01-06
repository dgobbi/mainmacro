project(mainmacro)

if(WIN32)
  set(MAINMACRO_SRC mainmacro.cpp)
else()
  set(MAINMACRO_SRC)
endif()

add_executable(example example.cpp ${MAINMACRO_SRC})
