Building 

1. Clone the repository and change into source directory.
   
    .. code:: bash
     git clone https://github.com/01-ai/Descartes.git
     cd Descartes/cpp

2. Run CMake to configure the build tree.

    .. code:: bash

      mkdir build && cd build
      cmake ../

3. Afterwards, generated files can be used to compile the project.

     .. code:: bash

       cmake --build build


4. Run the example.

     .. code:: bash
      
        wget http://ann-benchmarks.com/sift-128-euclidean.hdf5 
        ./examples/builder --config_file ../config/sift.cfg --dataset_file ./sift-128-euclidean.hdf5  --search_res_cnt 20
       
