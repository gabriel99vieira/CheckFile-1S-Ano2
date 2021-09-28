# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.05] - 2018


# Changes

Makefile > Line 64 > package name
```
--set-package=$(PROGRAM)
```


Makefile >End of file > generate only args
```
args:
	gengetopt < $(PROGRAM_OPT).ggo --file-name=$(PROGRAM_OPT) --set-package=$(PROGRAM)
```

