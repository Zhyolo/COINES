#!/usr/bin/env python3

"""Python wrapper for coinesAPI

`coinespy` allows users to access the Bosch Sensortec Application Board using Python

- Control VDD and VDDIO of sensor
- Configure SPI and I2C bus parameters
- Read and write into registers of sensors from Bosch Sensortec via SPI and I2C
- Read and write digital pins of the Application Board.

"""

import setuptools

DOCLINES = (__doc__ or '').split("\n")

setuptools.setup(
    name="coinespy",
    version="0.8.1",
    author="Bosch Sensortec GmbH",
    author_email="contact@bosch-sensortec.com",
    description=DOCLINES[0],
    long_description="\n".join(DOCLINES[2:]),
    url="https://www.bosch-sensortec.com/",
    packages=['coinespy'],
    package_data={'coinespy': ['libcoines_64.dll',
                               'libcoines_32.dll',
                               'libcoines_64.so',
                               'libcoines_32.so',
                               'libcoines.dylib',
                               'libcoines_armv7_32.so']
                  },
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 2",
        "License :: OSI Approved :: BSD License",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux",
        "Operating System :: MacOS :: MacOS X"
    ],
    python_requires='>=2.6, <4',
)