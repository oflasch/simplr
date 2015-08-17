SimplR provides basic tools for symbolic expression simplification, e.g.
simplify(x*1) => x, or simplify(sin(x)^2+cos(x)^2) => 1.
SimplR is based on the "Expression v3" (Ev3) 1.0 system by Leo Liberti.

This package contains the following objects:
.
├── .git                 git repository structure
│   └── [...]
├── .gitignore           list of files that git shouldn't track
├── LICENSE.txt          SimplR is distributed under the Common Public License
├── Makefile             the make script for building, packaging and testing SimplR 
├── README.txt           (this file)
├── bump-version         script to set the SimplR version tag
├── roxygenize           script for generating documentation from R sources
└── skel                 the R CRAN package skeleton
    ├── DESCRIPTION      metadata for the SimplR R package
    ├── R                R sources
    ├── inst             other SimplR components (e.g. unit tests) 
    └── src              C++ sources 

SimplR is copyright 2015 Oliver Flasch and Felix Gorschlueter. All rights reserved.
Released under the Common Public Licencse 1.0. See LICENSE.txt for details.

