# SchedViz


This repository contains the modules and scripts used to analyze thread blocking times in Linux v5.17.  
This work was carried out as part of Baptiste Pires' PhD thesis at the LIP6 in the Delys team. The thesis director is Pierre Sens, and the supervisors are : Swan Dubois, Redha Gouicem and Julien Sopena. This thesis is part of the  BPI Project - FogSLA‑Antillas project (DOS0168403/00‑DOS0168405/00).

## `module/`

This directory contains the code for the Linux kernel module to be inserted, which will perform the measurements and store the data. A patched Linux kernel v5.17 is required, located in the `kernel/` directory.

## `user/`

This directory contains all the userspace code used to run, measure, analyze, and generate graphs.

The `monitor_cmd.sh` script allows you to monitor a command:  
1. The module is inserted.  
2. The command passed as a parameter is executed, and its processes/threads are tagged to enable tracking during execution.  
3. At the end of the command’s execution, data is retrieved via `sysfs`.  
4. Finally, the data can be analyzed.  

> Several graphs can be generated, for example:

---

### Graph showing thread blocking events

- **X-axis**: Wake-up time  
- **Y-axis**: Blocking duration  
- **Color**: Thread type  
  - `benchmark` (our application)  
  - `other` (userspace threads not part of our app)  
  - `kernel`

#### Profile for `apache-siege`
---
![apache-siege](res/ths_apache-siege.png)

#### Profile for `compress-7zip`
---
![compress-7zip](res/ths_compress.png)

It is also possible to generate cumulative blocking graphs to better visualize the distribution of these events, as shown in the following chart:

### Cumulative blocking graph for the `hackbench` application
---
![hackbench](user/cumulative.png)

## `kernel/`

Patched Linux kernel `v5.17` required for inserting the module.

