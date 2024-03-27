# encoding=utf8
import getopt
import sys
import time

from py01ai import GraphIndex

import h5py

config = './cpp/config/sift.cfg'
data = 'sift-128-euclidean.hdf5'
search_res_cnt = 40
topk = 10

opts, args = getopt.getopt(sys.argv[1:], "hc:d:s:t:", ["help", "config_file=", "dataset_file=",
                                                       "search_res_cnt=", "topk="])
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
    elif opt in ('-t', '--topk'):
        topk = int(arg)

h5data = h5py.File(data)

index = GraphIndex()
ret = index.init(config)
if ret != 0:
    sys.stderr.write('index init failed\n')
    exit(1)

train_data = h5data['train']

ret = index.add_vector_parallel(train_data, 16)
if ret != 0:
    sys.stderr.write('add vector failed, ret: {}\n'.format(ret))
    exit(1)

ret = index.refine_index(True)
if ret != 0:
    sys.stderr.write('index refine failed\n')

ret = index.dump()
if ret != 0:
    sys.stderr.write('index dump failed\n')

print('current total count:', index.get_current_doc_cnt())

test_data = h5data['test']
label_data = h5data['neighbors']
total = 0.0

total_duration = 0

for i, v in enumerate(test_data):
    if i % 1000 == 0:
        print('searching {}-th vector ...'.format(i))
    explore_factor = 1.0
    start = time.perf_counter_ns()
    ret, entities = index.search(v, topk, search_res_cnt=search_res_cnt,
                                 explore_factor=explore_factor)
    total_duration += time.perf_counter_ns() - start
    if ret != 0:
        sys.stderr.write('search failed\n')
    key_set = set()
    for entity in entities:
        key, score = entity
        key_set.add(key)
    inter = set(label_data[i, 0:topk]) & key_set
    total += len(inter) / topk

print('query count {}. topk {}. recall {}. average {} us.'.format(test_data.shape[0], topk,
        total / test_data.shape[0], total_duration // test_data.shape[0] // 1000))
