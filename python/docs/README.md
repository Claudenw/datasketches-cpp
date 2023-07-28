Follow these steps to build the documentation.
1. Clone the directory in an appropriate location `git clone https://github.com/apache/datasketches-cpp.git` and navigate into `datasketches-cpp`.
2. Switch to the correct branch: `git checkout python-docs`.
3. *Either in a new virtual environment, or your current environment, install sphinx and datasketches via* 
(nb my environment has python aliased to python3 so just use whichever is appropriate for your installation)
```
python -m venv python-docs-venv # create a new virtual env named python-docs-venv
source python-docs-venv/bin/activate
python -m pip install sphinx 
python -m pip install sphinx-rtd-theme
```
4. In project root run `python3 -m pip install .` to build the python bindings.
5. Build and open the documentation:
```
cd python/docs
make html
open build/html/index.html
```
