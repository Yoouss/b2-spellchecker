## Benchmarks

The `benchmark` directory contains dictionaries and inputs to mesure the performances of our program, which will be writen in a file `benchmark.csv`

- **To generate the benchmarks of the detection mode**

```bash
make benchmark-detection
```

- **To generate the benchmarks of the correction mode**

```bash
make benchmark-correction
```

### How do we mesure the performances ?

We use :

- A 20 000 lines file for the detection 
- A 250 lines file for the correction
- 6 dictionaries

To mesure in 3 loops :

- The cpu pourcentage
- The used ram 
- The time

(Images comming soon)