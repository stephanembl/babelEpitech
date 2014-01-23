cmake_minimum_required(VERSION 2.8.6)
project(server C CXX)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -Werror")
endif()

if(WIN32)
	set(SYSTEM windows)
	set(MUTEX ../sources-shared/windows/CWMutex.cpp) 
	set(EXECUTABLE_OUTPUT_PATH ../)
	add_definitions(-D_WIN32_WINNT=0x0501)
else(WIN32)
	set(SYSTEM linux)
	set(MUTEX ../sources-shared/linux/CUMutex.cpp) 
endif(WIN32)

include_directories(			../headers-shared/shared 
								../headers-shared/${SYSTEM} 
								./headers/shared 
								./headers/${SYSTEM} 
								.)
add_executable( ${PROJECT_NAME}	../sources-shared/shared/LibC.cpp 
								../sources-shared/shared/Exception.cpp 
								../sources-shared/shared/Packet.cpp 
								../sources-shared/shared/PacketFactory.cpp 
								../sources-shared/shared/CircularBuffer.cpp 
								../sources-shared/shared/Mutex.cpp 
								../sources-shared/shared/MutexPool.cpp 
								${MUTEX} 
								./sources/shared/Network.cpp 
								./sources/shared/Client.cpp 
								./sources/shared/Server.cpp 
								./sources/shared/Account.cpp 
								./sources/shared/ManagerAccount.cpp 
								./sources/main.cpp)

## --- BOOST ---
set( Boost_USE_STATIC_LIBS		ON )
set( Boost_USE_MULTITHREADED	ON )
set( Boost_USE_STATIC_RUNTIME	OFF )
set( Boost_DEBUG        		OFF )
find_package( Boost COMPONENTS system date_time regex filesystem serialization REQUIRED )
if(Boost_FOUND)
	if (Boost_DEBUG)
		message(STATUS "Boost Version:" ${Boost_VERSION})
		message(STATUS "Boost Include Dirs:" ${Boost_INCLUDE_DIR})
		message(STATUS "Boost Root:" ${BOOST_ROOT})
		message(STATUS "Boost Libraries:")
		foreach(lib ${Boost_LIBRARIES})
				message(STATUS ${lib})
		endforeach()
	endif (Boost_DEBUG)
	include_directories( ${Boost_INCLUDE_DIRS} )
	link_directories( ${Boost_LIBRARY_DIRS} )
	target_link_libraries( ${PROJECT_NAME} ${Boost_LIBRARIES} )
endif(Boost_FOUND)
## --- !BOOST ---

## --- THREADS ---
if(UNIX)
	FIND_PACKAGE ( Threads REQUIRED )
	TARGET_LINK_LIBRARIES ( ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} )
endif(UNIX)
## --- !THREADS ---
