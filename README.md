# Ladder AMK Encoder

Use `make clean && make` to clean build the `ladder_amk_enc` in the default `build` sub-directory (also can use `make` to simplifier the build process, but may cause some runtime errors). 

Main syntax: `./ladder_amk_enc <number_of_elements> [option_1, option_2, ...]`

Use `./ladder_amk_enc --help` to see all the avaiable options.

Our experimental results can be found in ./build/report

The experimental results for each encoding are stored in a corresponding text file, using the format `{<number of elements> - <width of AMK constraints> - <K value>} : <solve-time>`.
