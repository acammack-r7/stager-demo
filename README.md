The +1 stager demo of h4x
=========================

To run (requires Linux):
  * `git clone`
  * `git submodule init`
  * `git submodule update`
  * `make` (or `make all32` for 32-bit binaries)
  * `./loader test.bin test` (or the 32-bit variants)
  * You should see `Hello, world!`


TODO
----

* Convert the C code in `loader.c` into shell code (generate with the metadata from the ELF file)
* Integrate with mettle
* ???
* Profit
