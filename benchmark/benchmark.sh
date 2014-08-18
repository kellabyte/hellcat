#!/bin/bash
echo "Set Sequential"
echo "===================="
../lib/Haywire/bin/wrk/wrk --script ./set_sequential.lua --latency -d 200 -t 1 -c 1 http://127.0.0.1:8000 -- 1

echo "Set Random"
echo "===================="
../lib/Haywire/bin/wrk/wrk --script ./set_random.lua --latency -d 10 -t 8 -c 16 http://127.0.0.1:8000 -- 8

echo "Get Sequential"
echo "===================="
../lib/Haywire/bin/wrk/wrk --script ./get_sequential.lua --latency -d 10 -t 8 -c 16 http://127.0.0.1:8000 -- 8

echo "Get Random"
echo "===================="
../lib/Haywire/bin/wrk/wrk --script ./get_random.lua --latency -d 10 -t 8 -c 16 http://127.0.0.1:8000 -- 8
