# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")
set (EXE_TARGET rtos)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rtos C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# To write your .c functions. 
#       1) Comment (#) the (.obj) file
#       2) Uncomment the source (.c) 
#       3) Write the code for the source file and compile.
        add_executable( ${EXE_TARGET} 
                        ./src/main.c
                        
			# These are the object files
                         #./obj/systick.c.obj
                         #./obj/tasks.c.obj
                         #./obj/add_task.c.obj
                         #./obj/remove_task.c.obj
                         #./obj/scheduler.c.obj
						 #./obj/cmdshell.c.obj
                        
                        # These are the source files
                        # To use comment (#) the corresponding obj file
                        #  and uncomment ( ) the corresponding c file 

                        ./src/systick.c
                        ./src/tasks.c
                        ./src/add_task.c
                        ./src/remove_task.c
                        ./src/scheduler.c
                        ./src/cmdshell.c
                        #
                        
                        
                        # Required For RTOS (Do not modify)
                        ./obj/context_switch.c.obj
						./obj/HAL.c.obj
						./obj/helper_functions.c.obj
		)

# Add the standard library to the build
target_link_libraries(rtos
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(${EXE_TARGET} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(${EXE_TARGET} 
        hardware_spi
        pico_multicore
        pico_malloc
        pico_mem_ops
        )

# Modify the below lines to enable/disable output over UART/USB
# Enable usb output, disable uart output
pico_enable_stdio_usb(${EXE_TARGET} 1)
pico_enable_stdio_uart(${EXE_TARGET} 0)

# create map/bin/hex file etc
pico_add_extra_outputs(${EXE_TARGET})

pico_set_program_name(${EXE_TARGET} "rtos")
pico_set_program_version(${EXE_TARGET} "2026")

# end of the file

