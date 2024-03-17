1. git  clone https://github.com/01-ai/Descartes.git  
2. git  clone https://github.com/erikbern/ann-benchmarks.git 
3. cp -r Descartes/ann-algo/ ann-benchmarks/ann_benchmarks/algorithms/descartes
4. cd ann-benchmarks/
5. sudo pip3 install -r requirements.txt
6. sudo python3 install.py --algorithm  descartes
7. sudo python3 run.py --dataset glove-25-angular --parallelism 31 --force --algorithm  'descartes(01AI)'
