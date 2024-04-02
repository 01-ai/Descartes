# Descartes Server

1. build

```bash
docker build --build-arg SSH_KEY="`cat ~/.ssh/id_rsa`" -t yourname/descartes_server .
```

2. start server

```bash
docker run -p 8080:8080 -v ./cpp/config:/config yourname/descartes_server -config /config/sift.cfg -listening_port 8080 -log_dir ./logs/
```

3. help options

```bash
docker run yourname/descartes_server -help
```

4. test request

```bash
# add vector
curl -d '{"rows":[{"key":0,"vector":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}]}' 127.0.0.1:8080/add_vector
# search
curl -d '{"topk":10,"search_res_cnt":10,"vector":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}' 127.0.0.1:8080/search
# refine
curl -X PUT -d '{"quantize": true}' 127.0.0.1:8080/refine_index
# dump
curl -X PUT 127.0.0.1:8080/dump
# get total count
curl 127.0.0.1:8080/current_doc_cnt
```
