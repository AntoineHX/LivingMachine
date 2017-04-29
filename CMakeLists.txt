cmake_minimum_required(VERSION 2.8)


#Configuration du projet

project(DisplayImage)

#Configuration de la sortie

set(EXECUTABLE_OUTPUT_PATH bin)

#Configuration de l'exécutable

file(
        GLOB_RECURSE

        source_files

        src/*
)

#Recherche des bibliothèques

find_package( OpenCV REQUIRED )
#find_package(SFML 2 COMPONENTS system window graphics audio REQUIRED)

add_executable( DisplayImage ${source_files} )
target_link_libraries( DisplayImage ${OpenCV_LIBS} )

