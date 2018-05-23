import setuptools

with open("README.rst", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="pickle5",
    version="0.0.1",
    author="Antoine Pitrou",
    author_email="antoine@python.org",
    description="Experimental backport of the pickle 5 protocol (PEP 574)",
    long_description=long_description,
    long_description_content_type="text/x-rst",
    url="https://github.com/pitrou/pickle5-backport",
    packages=setuptools.find_packages(),
    classifiers=(
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Python Software Foundation License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: Implementation :: CPython",
    ),
)
