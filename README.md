# imputed-data-dynamic-threshold

[![CircleCI](https://dl.circleci.com/status-badge/img/gh/lightning-auriga/imputed-data-dynamic-threshold/tree/default.svg?style=svg)](https://dl.circleci.com/status-badge/redirect/gh/lightning-auriga/imputed-data-dynamic-threshold/tree/default)

[![codecov](https://codecov.io/gh/lightning-auriga/imputed-data-dynamic-threshold/graph/badge.svg?token=31FA25tWe9)](https://codecov.io/gh/lightning-auriga/imputed-data-dynamic-threshold)

## Brief Summary

compute r2 threshold using Yun Li cleaning method

## Overview

Traditionally, post-imputation variant filtering has been applied with
a single, constant r<sup>2</sup> cutoff applied to all variants. Over the last decade,
one of the original creators of the mach/minimac family of imputation tools
has switched to a more nuanced model of filtering where different r<sup>2</sup> filters
are selected for different minor allele frequency bins based on the desired
per-bin post-filtering average r<sup>2</sup>. This package implements the calculation
of those bin-specific filters.

## Installation

There are two primary installation methods, depending on your needs.

### With conda (recommended)

  - If needed, [install mamba](https://mamba.readthedocs.io/en/latest/mamba-installation.html#mamba-install)
  - Install the package with mamba:

    `mamba create -n iddt -c https://raw.githubusercontent.com/lightning-auriga/conda-builds/default/conda-builds -c conda-forge imputed-data-dynamic-threshold`
  - Activate the resulting environment:

    `mamba activate iddt`
  - The tool should now be available as `imputed-data-dynamic-threshold.out`

### Manual build

#### Requirements

  - g++ >= 8.2.0
  - automake/autoconf
  - make >= 4.2
  - git >= 2.28.0
  - nodejs (for commitizen)
  - pre-commit
  - associated linting tools for C++: cppcheck, clang-format
  - [boost headers](https://www.boost.org)
  - [boost program_options](https://www.boost.org/doc/libs/1_82_0/doc/html/program_options.html)
  - [boost filesystem/system](https://www.boost.org/doc/libs/1_82_0/libs/filesystem/doc/index.htm)
  - [htslib](https://github.com/samtools/htslib)
  - [zlib](https://zlib.net)
  - [doxygen](https://www.doxygen.nl/index.html) (only required for rebuilding inline documentation)

### Build

By default, a build process involving a [conda](https://docs.conda.io/en/latest/) environment is supported.

  - If needed, [install mamba](https://mamba.readthedocs.io/en/latest/mamba-installation.html#mamba-install)
  - navigate into your project directory (imputed-data-dynamic-threshold)
  - create the `conda` environment for installation as follows:

     `mamba env create -f environment.yaml`
  - activate the conda environment:

     `conda activate imputed-data-dynamic-threshold-env`
  - (one time only per environment) install `commitizen`:

     `npm install -g commitizen cz-conventional-changelog`
  - (one time only per environment) install `pre-commit` linters:

     `pre-commit install`

  - update (create) the necessary `configure` scripts with `autoreconf`:

     `autoreconf --force --install`

     - note that this can also be run with `./generate.bash` inside the repo
  - run `configure`:

	 `./configure --with-boost=${CONDA_PREFIX} --with-boost-libdir=${CONDA_PREFIX}/lib`

	 - if you are planning on installing software to a local directory, run instead `./configure --prefix=/install/dir [...]`
	 - periodically there are some incompatibility issues between `configure` and `conda`. if so, you may need to override
	   some default locations detected by `configure`. for example, you might override the detected compiler with:
	   `CC=gcc CXX=g++ ./configure [...]`
  - run `make`
  - run optional `./test-suite.out` to run unit tests
  - if desired, run `make install`. if permissions issues are reported, see above for reconfiguring with `./configure --prefix`.

## Usage

By default, the final compiled program can be run with

`imputed-data-dynamic-threshold.out`

The following command line options are supported with this software:

|Option|Description|
|---|---|
|-h<br>--help|print in-terminal help text describing these accepted parameters.|
|-i<br>--info-gz-files|specify minimac4-format `info.gz` files for processing with this software. file extension is not checked, and flat files that have already been extracted are supported. only variants tagged as `Imputed` in info column 8 are considered for this filtering criterion. it is anticipated that, for example, all autosomal info files for a single imputation will be in one directory, so they can all be specified to the software at once as `-i /path/to/files/*info.gz`.|
|-v<br>--vcf-files|specify vcf files for processing with this software. file extension is not checked, but contents are verified by [htslib](https://github.com/samtools/htslib). INFO fields corresponding to whether the variant was imputed, imputation r<sup>2</sup>, and allele frequency are rapidly parsed and processed. it is anticipated that, for example, all autosomal vcfs for a single imputation will be in one directory, so they can all be specified to the software at once as `-v /path/to/files/*vcf.gz`.|
|--vcf-info-r2-tag|name of INFO tag with imputation r<sup>2</sup>. defaults to beagle `DR2`.|
|--vcf-info-af-tag|name of INFO tag with allele frequency. defaults to beagle `AF`. this field anticipates biallelic variants, and will have problematic behaviors otherwise.|
|--vcf-info-imputed-indicator|name of INFO tag indicating that a variant was imputed from a reference. defaults to beagle `IMP`.|
|-m<br>--maf-bin-boundaries|definition of minor allele frequency bins in which to compute separate r<sup>2</sup> thresholds. bounds should be strictly increasing decimal values on [0,1]. the arguments are interpreted as follows: the specification `-m 0.001 0.005 0.01 0.03 0.05 0.5` is converted into the frequency bins `(0.001, 0.005]`, `(0.005, 0.01]`, `(0.01, 0.03]`, `(0.03, 0.05]`, `(0.05, 0.5]`. variants with allele frequencies falling below the minimum bound or above the maximum bound of the provided bins are excluded from consideration entirely. note that a maximum bound of 0.5 captures all variation on that end as these are _minor_ allele frequencies. if not specified, this defaults to the values `-m 0.001 0.005 0.01 0.03 0.05 0.5` as specified in `doi:10.1002/gepi.21603`.|
|-o<br>--output-table|name of file in which to store tabular output summary. if not specified, results will be printed to terminal. output format is tab-delimited plaintext, one row per frequency bin, with the following columns:<br>`bin_min`: minimum minor allele frequency, exclusive, of the specified bin<br>`bin_max`: maximum minor allele frequency, inclusive, of the specified bin<br>`total_variants`: number of variants in bin before dynamic filtering<br>`threshold`: dynamic filter applied to bin to reach desired average r<sup>2</sup>. this entry can be `nan`, in which case the desired average r<sup>2</sup> is greater than the maximum r<sup>2</sup> of variants falling within this bin (or the bin is empty to begin with)<br>`variants_after_filter`: number of variants in bin after dynamic filtering<br>`proportion_passing`: proportion of variants passing dynamic filter|
|-l<br>--output-list|name of file in which to store variants passing filters, along with typed variation from input info files. if not specified, list is not generated.|
|-s<br>--second-pass|for variant list reporting: whether to skip ID storage during threshold calculation, and instead perform a second pass of all the info files once the thresholds have been computed. this substantially reduces the RAM usage of the software, at the cost of file parsing time.|
|--filter-info-files|path to a directory. if desired, the software can emit output minimac-format info files with computed variant filters applied. for the moment, the output filename structure is not user configurable (will be: `/target/path/chr*.info.gz`). this option only works if `--second-pass` is enabled; otherwise, it is ignored.|
|--filter-vcf-files|placeholder; not yet supported.|
|-r<br>--target-average-r2|desired average r<sup>2</sup> within bin after dynamic filtering. this should be a value on [0, 1], though values on [0, 0.3] will effectively suppress dynamic filtering, as a flat minimum r<sup>2</sup> filter of 0.3 is applied to all variants. defaults to `-r 0.9`.|

## Version History

21 10 2023:
  - future version updates migrated to ChangeLog

20 10 2023:
  - update dusty old repo, assign license, and release

XX 05 2022:
  - add option to filter and write modified info files during second pass
  - add/improve unit tests with cppunit
  - v1.1.0

27 09 2021: added variant list reporting; v1.0.2

25 09 2021: project generated from cookiecutter template
