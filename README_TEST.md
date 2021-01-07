Running tests requires Catch2 and cmake

# Running Tests
```sh
mkdir cbuild && cd cbuild
cmake ..
make test

# usage: run all tests
./test/test

# usage: Optionally specify tests and success verbosity
./test/test [bass] --success
```

# Writing Tests
`[noci]` tag is used to disable a test on Github actions
