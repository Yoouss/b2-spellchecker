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
---

To evaluate performance, we analyze the following metrics over three consecutive loops :

    - CPU usage (%)
    - RAM consumption
    - Execution time

Using :

    - A 20 000-line file for the detection mode
    - A 250-line file for the correction mode
    - 6 Dictionaries for both modes

### RPI4 benchmarks' results
---

- **Detection mode**

<img src="./rpi4_detection_benchmark.png" alt="Detection's benchmarks" width=750>

- **Correction mode**

<img src="./rpi4_correction_benchmark.png" alt="Correction's benchmarks" width=750>

### Interpretation
---

**We are aware of the anomaly in the correction mode when using 2 threads**; however we aren't sure why this happens... 

<img src="./benchmarks_execution_time_chart.png" alt="Correction's benchmarks" width=700>

**We suspect that we didn't use the threads properly**, there's still significant computational load required to find the candidate for each word's correction <br>

We believe that **implementing multithreading for this specific part is challenging**, as it is difficult **to guarantee that an even distribution of the workload across each threads** <br>

Overall we believe we have achieved **good spatial performance** and a **decent execution speed**