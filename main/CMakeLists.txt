# include all c/cpp files in main-directory here:

idf_component_register(SRCS "main.cpp"
                            "si7021.cpp"
                            "sdCard.cpp"
                            INCLUDE_DIRS ".")
