# include all c/cpp files in main-directory here:

idf_component_register(SRCS "main.cpp"
                            "testClass.cpp"
                            "ws2812.cpp"
                            INCLUDE_DIRS ".")
