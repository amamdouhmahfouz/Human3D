# Human 3D

### Installation

make sure to have [Eigen 3.3](https://eigen.tuxfamily.org/index.php?title=Main_Page) and [HDF5](https://www.hdfgroup.org/downloads/hdf5/) installed on your system (see instructions in each link)

```{sh}
git clone <repo link.git>
cd Human3D
mkdir build && cd build
cmake ..
make
```

### Download Model

male model: https://drive.google.com/file/d/1h205gGAISJvnZ7SVAs76kCl7tvJdz-57/view?usp=sharing \
female model: https://drive.google.com/file/d/1bwuzcNGEenVWHWQhbVpbxe153ki_UMor/view?usp=sharing
> put the downloaded model in the models folder

### Usage
***make sure you are in the build directory***

Fit shape model on input json with proportions:
```{sh}
./fitting ../models/pcaModel_male_630_smplx_v2_T.h5 ../data/referenceObj.obj ../data/ids_index_v2.json ../data/inputParamsjson ../output/finalMesh7.obj ../output/finalLms7.json
```

Build your own shape model from a directory:
```{sh}
./buildShapeModel path_to_meshes_dir ../data/referenceObj.obj output_path_for_h5_model 
```

### Datasets
You can use your own acquired datasets or one of the datasets we have used 

1. [SPRING](https://graphics.soe.ucsc.edu/data/BodyModels/index.html)
2. Generate your own from [SMPLX](https://github.com/vchoutas/smplx) (see the examples/demo.py in their repo)