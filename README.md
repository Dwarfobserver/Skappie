
# Skappie

This is a personal project : a 2D network game engine.

It's developped with MSVC and Visual Studio and uses recent C++ features.

Dependencies (stored in the repository) :
 - Catch (for tests)
 - SFML (for client window, graphic and sound)
 - Asio (for asynchronous UDP socket)

The solution has four projects :
 - common, a library used by all three other projects
 - client, the Skappie client
 - server, the Skappie server (gnn)
 - tests, source files filled of tests baseds on the common library
