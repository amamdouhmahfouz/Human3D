# Human 3D

### Installation
```{sh}
git clone <repo link.git>
cd Human3D
mkdir build && cd build
cmake ..
make
```

### Download Model

link: https://drive.google.com/file/d/1cv11i8VgONJJbzgVjtW-5gsOFckY-5jd/view?usp=sharing
> put the downloaded model in the models folder

### Usage
***make sure you are in the build directory***

Fit shape model on input json with proportions:
```{sh}
./fitting ../models/pcaModel_male_630_smplx_v2_T.h5 ../data/referenceObj.obj ../data/ids_index_v2.json ../data/inputParamsjson ../output/finalMesh7.obj ../output/finalLms7.json
```

Build a shape model from a directory:
```{sh}
./buildShapeModel path_to_meshes_dir ../data/referenceObj.obj output_path_for_h5_model 
```