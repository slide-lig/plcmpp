# pLCM++

A fast C++ implementation implementation of the LCM (Linear Closed itemsets Miner) algorithm, as proposed by T.Uno & H.Arimura. It is multi-threaded, as proposed by Négrevergne et al., hence the name of its main class: PLCM.

The pLCM++ implementation started as a port of [jLCM](https://github.com/martinkirch/jlcm) to C++.
jLCM is a parallel implementation of the LCM algorithm, in java, 
and was mostly written by Martin Kirchgessner and Vincent Leroy.

Reference papers:

* "An efficient algorithm for enumerating closed patterns in transaction 
databases" by T. Uno, T. Asai, Y. Uchida and H. Arimura, in Discovery Science, 
2004
* "Discovering closed frequent itemsets on multicore: Parallelizing computations 
and optimizing memory accesses" by B. Negrevergne, A. Termier, J-F. Mehaut, 
and T. Uno in International Conference on High Performance Computing & 
Simulation, 2010


## Performance
- pLCM++ is usually around 3 times faster than [jLCM](https://github.com/martinkirch/jlcm) on common datasets
(main improvements over jLCM are described below)
- when limited to 1 thread (i.e. using the command line option `-t 1`),
the performance of pLCM++ is usually similar to the one of [LCM 2.5](http://research.nii.ac.jp/~uno/code/lcm25.zip),
the fastest mono-threaded implementation proposed by T. Uno. 


## Compiling and running pLCM++

Use `make` to build.
Run `./pLCM++ --help` for usage details.

This tool uses ASCII files as input: each line represents a transaction. You may find example input files in the [FIMI repository](http://fimi.ua.ac.be/data/).


## License and copyright owners

This work is released under the Apache License 2.0 (see LICENSE).

Copyright 2013 Etienne Dublé, Martin Kirchgessner, Vincent Leroy, Alexandre Termier, 
CNRS and Université Joseph Fourier.


## Improvements over jLCM

After the initial porting phase, pLCM++ received a set of 
optimizations. Main ones are:
- Use of minimal integer types in the processing part, not only
for storage (as it was in [jLCM](https://github.com/martinkirch/jlcm)). This allows more optimization
opportunities for the compiler.
- Improved dataset reduction algorithm: pLCM++ uses hashes in 
order to find which transactions can be merged together.
- Prefer range-based over java-style iterators (i.e. provide 
start and end of a range instead of an abstract Iterator object
with methods hasNext() and next()). This is obviously faster,
but it required algorithmic changes in order to provide contiguous
ranges when possible.



