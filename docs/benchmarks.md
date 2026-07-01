## Benchmarks

The `benchmark` directory contains dictionaries and inputs to measure the performances of our program, which will be written in a file `benchmark.csv`

- **To generate the benchmarks of the detection mode**

```bash
make benchmark-detection
```

- **To generate the benchmarks of the correction mode**

```bash
make benchmark-correction
```

### How do we measure the performances ?

We use :

- A 20 000 lines file for the detection 
- A 250 lines file for the correction
- 6 dictionaries

To measure in 3 loops :

- The cpu percentage
- The used ram 
- The time

(Images coming soon)