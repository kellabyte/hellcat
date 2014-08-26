[![Build Status](https://travis-ci.org/kellabyte/hellcat.png?branch=master)](https://travis-ci.org/hellcat/Haywire)

Hellcat
=======
Hellcat is a high performance database experiment project to see how fast of a database I can write in C and C++. My other high performance experiment project [Haywire](https://github.com/kellabyte/Haywire) that can do `600,000` HTTP requests/second on old hardware is used in Hellcat to accept get and put requests over HTTP. 

## Goals

- Compete with Redis and Memcached in performance.
- Crash safe. Data does not get corrupted or lost as much as you can ensure with dirty lying disks.
- Ability to run with in-memory only mode or ACID mode with persistence and serializable isolation.
- Add Redis and Memcached client protocol compatibility support.
- SIMD hardware accelerated indexes for search queries.
- Replication.

## Contributors
I would love to work with others on this project to see what we can accomplish and to improve my C and C++ skills. If you're interested let me know!

