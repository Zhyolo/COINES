# `coinespy` library

The `coinespy` library allows users to access Bosch Sensortec's MEMS sensors on the Application Board 2.0 through a Python interface. It offers a flexible solution for developing a host independent wrapper interface for the sensors with robust error handling mechanism. The core functionalities remain the same as with usage of coinesAPI on C level.

This folder contains the Python wrapper on top of the COINES C library.

To build a new wheel, follow the steps as described in: https://packaging.python.org/tutorials/packaging-projects/

To install this package:

```bash
$ python setup.py install
```