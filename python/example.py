# encoding=utf8
import getopt
import sys

from pydescartes.descartes import GraphIndex, SearchContext

import h5py

config = './cpp/config/sift.cfg'
data = 'sift-128-euclidean.hdf5'
search_res_cnt = 60

opts, args = getopt.getopt(sys.argv[1:], "hc:d:s:", ["help", "config_file=", "dataset_file=",
                                                     "search_res_cnt="])
for opt, arg in opts:
    if opt in ('-h', '--help'):
        print('{} -c <config_file> -d <dataset_file> -s <search_res_cnt>'.format(sys.argv[0]))
        exit(0)
    elif opt in ('-c', '--config_file'):
        config = arg
    elif opt in ('-d', '--dataset_file'):
        data = arg
    elif opt in ('-s', '--search_res_cnt'):
        search_res_cnt = int(arg)

h5data = h5py.File(data)

index = GraphIndex()
ret = index.init(config)
if ret != 0:
    sys.stderr.write('index init failed\n')
    exit(1)

train_data = h5data['train']

for i in range(train_data.shape[0]):
    if i % 1000 == 0:
        print('adding {}-th vector ...'.format(i))
    index.add_vector(train_data[i, :], i)

ret = index.refine_index(True)
if ret != 0:
    sys.stderr.write('index refine failed\n')

ret = index.dump()
if ret != 0:
    sys.stderr.write('index dump failed\n')

print('current total count:', index.get_current_doc_cnt())

topk = 10

test_data = h5data['test']
label_data = h5data['neighbors']
total = 0.0
for i in range(test_data.shape[0]):
    if i % 1000 == 0:
        print('searching {}-th vector ...'.format(i))
    context = SearchContext()
    context.topk = topk
    context.search_res_cnt = search_res_cnt
    ret = index.search(test_data[i, :], context)
    if ret != 0:
        sys.stderr.write('search failed\n')
    key_set = set()
    for entity in context.get_result():
        key, score = entity.key, entity.score
        key_set.add(key)
    inter = set(label_data[i, 0:len(context.get_result())]) & key_set
    total += len(inter) / context.topk

print('query count {}. topk {}. recall {}'.format(test_data.shape[0], topk,
                                                  total / test_data.shape[0]))
