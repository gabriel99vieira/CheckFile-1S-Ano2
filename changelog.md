# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.05] - 2018

<br>

# Changes

## 2021-09-02

Makefile > Line 64 > package name

```
--set-package=$(PROGRAM)
```

<br>

Makefile > End of file > generate only args

```
args:
	gengetopt < $(PROGRAM_OPT).ggo --file-name=$(PROGRAM_OPT) --set-package=$(PROGRAM)
```

<br>

Added more `.h` (comented) depencencies do `main.c` file for easier use.

```c
// #include <stdlib.h>
// #include <unistd.h>
// #include <errno.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <sys/stat.h>
// #include <time.h>
// #include <fcntl.h>
// #include <stdint.h>
// #include <ctype.h>
// #include <signal.h>
// #include <sys/sysmacros.h>
// #include <sys/stat.h>
// #include <assert.h>
// #include <dirent.h>

// #include "debug.h"
// #include "memory.h"
// #include "args.h"
```

<br>

Added `-Wno-unknown-pragmas` flag to compiler

## New Change
