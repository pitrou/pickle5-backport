from setuptools import setup, Extension, find_packages


with open("README.rst", "r") as fh:
    long_description = fh.read()


ext_modules = [
    Extension('pickle5._pickle',
              ['pickle5/_pickle.c', 'pickle5/picklebufobject.c'],
              depends=['pickle5/picklebufobject.h',
                       'pickle5/compat.h']),
    ]

setup(
    name="pickle5",
    version="0.0.4",
    author="Antoine Pitrou",
    author_email="antoine@python.org",
    description="Experimental backport of the pickle 5 protocol (PEP 574)",
    long_description=long_description,
    long_description_content_type="text/x-rst",
    url="https://github.com/pitrou/pickle5-backport",
    packages=find_packages(),
    ext_modules=ext_modules,
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Python Software Foundation License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: Implementation :: CPython",
    ],
)
