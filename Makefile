# Makefile
# make build script for simplr 
#

R_HOME ?= $(shell R RHOME)
RELEASE_DATE = $(shell date +%Y-%m-%d)

.SILENT:
.PHONEY: usage help install test check clean distclean package

usage:
	echo "Makefile for the simplr symbolic expression simplification package for R."
	echo "Usage: make TARGET with TARGET being:"
	echo ""
	echo "  check         - Run CRAN check on the package."
	echo "  clean         - Clean up package cruft."
	echo "  distclean     - Clean up and remove all generated artifacts."
	echo "  help          - Show this message."
	echo "  install       - Install the package, writing the output into install.log."
	echo "  package       - Build source package of last commit."
	echo "  roxygen       - Roxygenize skel/ into pkg/."

help: usage

install: clean roxygen
	echo "Installing package..."
	"$(R_HOME)/bin/R" CMD install --no-multiarch pkg > install.log 2>&1 || cat install.log
	echo "DONE."

check: clean roxygen
	echo "Running CRAN check..."
	"$(R_HOME)/bin/R" CMD check --as-cran pkg && rm -fR pkg.Rcheck
	echo "DONE."

roxygen:
	echo "Roxygenizing package..."
	cp -r skel pkg
	./roxygenize > roxygen.log 2>&1 || cat roxygen.log
	echo "DONE."

clean:
	echo "Cleaning up..."
	rm -fR pkg
	rm -fR pkg.Rcheck
	rm -fR skel.Rcheck simplr.Rcheck
	rm -fR skel/src/*.o skel/src/*.so skel/R/*~
	rm -fR .RData .Rhistory build.log install.log roxygen.log
	echo "DONE."

distclean: clean
	echo "Removing all generated artifacts..."
	rm -f simplr_*.tar.gz 
	echo "DONE."

package: clean roxygen
	echo "Building package..."
	echo "Release date: $(RELEASE_DATE)"
	echo "Date: $(RELEASE_DATE)" >> pkg/DESCRIPTION
	git log --no-merges -M --date=iso pkg/ > pkg/ChangeLog
	"$(R_HOME)/bin/R" CMD build pkg > build.log 2>&1
	rm -fR pkg
	echo "DONE."

# EOF

