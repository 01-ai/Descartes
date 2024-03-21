# Descartes Python Document

1. install descartes library with pip

```bash
pip install lib/descartes-0.0.1-cp310-cp310-linux_x86_64.whl
```

2. get dependency and data for the example

```bash
wget http://ann-benchmarks.com/sift-128-euclidean.hdf5 
pip install h5py
```

3. run the example

```bash
python python/example.py -c cpp/config/sift.cfg -d sift-128-euclidean.hdf5 -s 40
```
