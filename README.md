# imputed-data-dynamic-threshold

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

## Requirements

  - g++ >= 8.2.0
  - automake/autoconf
  - make >= 4.2
  - git >= 2.28.0
  - nodejs (for commitizen)
  - pre-commit
  - associated linting tools for C++: cppcheck, clang-format
  - [boost headers](https://www.boost.org)
  - [boost program_options](https://www.boost.org/doc/libs/1_75_0/doc/html/program_options.html)
  - [boost filesystem/system](https://www.boost.org/doc/libs/1_75_0/libs/filesystem/doc/index.htm)
  - [boost iostreams](https://www.boost.org/doc/libs/1_74_0/libs/iostreams/doc/index.html)
  - [yaml-cpp](https://github.com/jbeder/yaml-cpp)
  - [zlib](https://zlib.net)

## Build

By default, a build process involving a [conda](https://docs.conda.io/en/latest/) environment is supported.

  - if you wish to use `conda` and it's not currently available, you can install it with the instructions [here](https://docs.conda.io/en/latest/miniconda.html)
  - navigate into your project directory (imputed-data-dynamic-threshold)
  - create the `conda` environment for installation as follows:
  
     `conda env create -f environment.yaml`
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
  
	 `./configure --with-boost=/path/to/miniconda3/envs/imputed-data-dynamic-threshold-env --with-boost-libdir=/path/to/miniconda3/envs/imputed-data-dynamic-threshold-env/lib`

	 - if you are planning on installing software to a local directory, run instead `./configure --prefix=/install/dir [...]`
	 - periodically there are some incompatibility issues between `configure` and `conda`. if so, you may need to override
	   some default locations detected by `configure`. for example, you might override the detected compiler with:
	   `CC=gcc CXX=g++ ./configure [...]`
  - run `make CPPFLAGS=""`
	 - this is a non-standard `make` invocation. the reason this is included is because the project
	   is configured to specifically use a `boost` installation in the accompanying `conda` environment.
	   if you'd rather remove `boost` from the conda environment, or ignore it in favor of a system-wide
	   `boost` installation, you can adjust the appropriate `configure` parameters accordingly
	   and instead invoke `make` without any further variable overrides
  - run `make check` to run any `TAP/automake` tests, or the placeholder
     - if you run this command without compiling first, you will again need to override `CPPFLAGS`
	   as follows: `make CPPFLAGS="" check`

  - if desired, run `make install`. if permissions issues are reported, see above for reconfiguring with `./configure --prefix`.
     - as above, if you run installation without compiling first, you will again need to override `CPPFLAGS`
	   as follows: `make CPPFLAGS="" check`
  
## Usage

By default, the final compiled program can be run with

`./imputed-data-dynamic-threshold.out`

The following command line options are supported with this software:

- `-h, --help`: print in-terminal help text describing these accepted parameters.
- `-i, --info-gz-files [FILE ...]`: specify minimac4-format `info.gz` files for processing with this software. file extension is not checked, and flat files that have already been extracted are supported. only variants tagged as `Imputed` in info column 8 are considered for this filtering criterion. it is anticipated that, for example, all autosomal info files for a single imputation will be in one directory, so they can all be specified to the software at once as `-i /path/to/files/*info.gz`.
- `-m, --maf-bin-boundaries [BOUND1 BOUND2 ...]`: definition of minor allele frequency bins in which to compute separate r<sup>2</sup> thresholds. bounds should be strictly increasing decimal values on [0,1]. the arguments are interpreted as follows: the specification `-m 0.001 0.005 0.01 0.03 0.05 0.5` is converted into the frequency bins `(0.001, 0.005]`, `(0.005, 0.01]`, `(0.01, 0.03]`, `(0.03, 0.05]`, `(0.05, 0.5]`. variants with allele frequencies falling below the minimum bound or above the maximum bound of the provided bins are excluded from consideration entirely. note that a maximum bound of 0.5 captures all variation on that end as these are _minor_ allele frequencies. if not specified, this defaults to the values `-m 0.001 0.005 0.01 0.03 0.05 0.5` as specified in `doi:10.1002/gepi.21603`.
- `-o, --output-filename [FILE]`: name of file in which to store tabular output summary. output format is tab-delimited plaintext, one row per frequency bin, with the following columns:
  - `bin_min`: minimum minor allele frequency, exclusive, of the specified bin
  - `bin_max`: maximum minor allele frequency, inclusive, of the specified bin
  - `total_variants`: number of variants in bin before dynamic filtering
  - `threshold`: dynamic filter applied to bin to reach desired average r<sup>2</sup>. this entry can be `nan`, in which case the desired average r<sup>2</sup> is greater than the maximum r<sup>2</sup> of variants falling within this bin (or the bin is empty to begin with)
  - `variants_after_filter`: number of variants in bin after dynamic filtering
  - `proportion_passing`: proportion of variants passing dynamic filter
- `-r, --target-average-r2 [R2]`: desired average r<sup>2</sup> within bin after dynamic filtering. this should be a value on [0, 1], though values on [0, 0.3] will effectively suppress dynamic filtering, as a flat minimum r<sup>2</sup> filter of 0.3 is applied to all variants. defaults to `-r 0.9`.

## Future Development Targets
- [ ] report inclusion list of variants passing filter

## Version History

25 09 2021: project generated from cookiecutter template
