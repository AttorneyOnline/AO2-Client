Running tests requires Catch2 and cmake.  libs are assumed to be in
the same directory as the executable

```sh
mkdir cbuild && cd cbuild
cmake ..
make test

# usage: run all tests
./test/test

# usage: Optionally specify tests and success verbosity
./test/test [bass] --success
```
