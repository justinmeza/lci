# lci

lci is a [LOLCODE](http://lolcode.org) interpreter written in C and is designed
to be correct, portable, fast, and precisely documented.

- **Correct**: Every effort has been made to test lci's conformance to the
  LOLCODE language specification. Unit tests come packaged with the lci source
  code.

- **Portable**: lci follows the widely ported ANSI C specification allowing it
  to compile on a broad range of systems.

- **Fast**: Much effort has gone into producing simple and efficient code
  whenever possible to the extent that the above points are not compromised.

- **Precisely documented**: lci uses Doxygen to generate literate code
  documentation, browsable here.

This project's homepage is at http://lolcode.org.  For help, visit
http://groups.google.com/group/lci-general.  To report a bug, go to
http://github.com/justinmeza/lci/issues.

Created and maintained by Justin J. Meza (http://justinmeza.com).

> Looking for the LOLCODE 1.3 spec implementation? See the
> [`v1.3` tag](https://github.com/justinmeza/lci/tree/v1.3).

## Installation

### Prerequisites

1. [CMake](http://www.cmake.org) (if using a Linux distro with package
   management, CMake should be in your repositories).

2. Python 2.7+ or Python 2.x with the argparse module installed.

### The Easy Way (Linux / macOS)

Run the install script:

```
$ ./install.py -h          # show install options
$ ./install.py             # install to default prefix
$ ./install.py --prefix="/home/foo/opt"  # custom prefix
```

### The Manual Way (Linux / macOS)

```
$ cmake .                  # configure
$ make                     # build
$ make install             # install
$ ctest                    # run tests (optional)
$ make docs                # build docs (optional)
```

To enable memory testing:

```
$ cmake -DPERFORM_MEM_TESTS:BOOL=ON .
```

## Standard Library

lci includes a binding system that exposes C libraries to LOLCODE via
`CAN HAS <library>?`. Four libraries are available:

### STDIO — File I/O

```
CAN HAS STDIO?
```

| Function | Arguments | Description |
|----------|-----------|-------------|
| `STDIO'Z OPEN` | filename, mode | Open a file (`"r"`, `"w"`, etc.) |
| `STDIO'Z LUK` | file, length | Read up to *length* bytes |
| `STDIO'Z SCRIBBEL` | file, data | Write a string to a file |
| `STDIO'Z CLOSE` | file | Close a file handle |
| `STDIO'Z DIAF` | file | Check for file error (returns TROOF) |
| `STDIO'Z AGEIN` | file | Rewind to the beginning of a file |

**Example — read a file:**

```lolcode
HAI 1.4
  CAN HAS STDIO?

  I HAS A file ITZ STDIO'Z OPEN "hello.txt" AN "r"
  I HAS A contents ITZ STDIO'Z LUK file AN 1024
  VISIBLE contents
  STDIO'Z CLOSE file
KTHXBYE
```

### SOCKS — Networking

```
CAN HAS SOCKS?
```

| Function | Arguments | Description |
|----------|-----------|-------------|
| `SOCKS'Z RESOLV` | addr | DNS lookup, returns IP string |
| `SOCKS'Z BIND` | addr, port | Open a TCP socket (use `"ANY"` for all interfaces) |
| `SOCKS'Z LISTN` | local | Accept an incoming connection |
| `SOCKS'Z KONN` | local, addr, port | Connect to a remote host |
| `SOCKS'Z CLOSE` | local | Close a socket |
| `SOCKS'Z PUT` | local, remote, data | Send data, returns bytes sent |
| `SOCKS'Z GET` | local, remote, amount | Receive up to *amount* bytes |

**Example — simple TCP server:**

```lolcode
HAI 1.4
  CAN HAS SOCKS?

  I HAS A srv ITZ SOCKS'Z BIND "ANY" AN 8080
  I HAS A client ITZ SOCKS'Z LISTN srv
  I HAS A msg ITZ SOCKS'Z GET srv AN client AN 1024
  VISIBLE msg
  SOCKS'Z PUT srv AN client AN "KTHXBAI"
  SOCKS'Z CLOSE srv
KTHXBYE
```

### STRING — String Operations

```
CAN HAS STRING?
```

| Function | Arguments | Description |
|----------|-----------|-------------|
| `STRING'Z LEN` | string | Return the length of a string |
| `STRING'Z AT` | string, position | Return the character at a 0-based index |

**Example:**

```lolcode
HAI 1.4
  CAN HAS STRING?

  I HAS A word ITZ "LOLCODE"
  VISIBLE STRING'Z LEN word        BTW 7
  VISIBLE STRING'Z AT word AN 0    BTW L
KTHXBYE
```

### STDLIB — Utilities

```
CAN HAS STDLIB?
```

| Function | Arguments | Description |
|----------|-----------|-------------|
| `STDLIB'Z MIX` | seed | Seed the random number generator |
| `STDLIB'Z BLOW` | max | Return a random integer in [0, max) |

**Example:**

```lolcode
HAI 1.4
  CAN HAS STDLIB?

  STDLIB'Z MIX 42
  VISIBLE STDLIB'Z BLOW 100    BTW random number 0-99
KTHXBYE
```

## BRAINZ — Neural Networks

The BRAINZ library provides a character-level transformer language model,
implemented entirely in C with automatic differentiation. It is designed
for training small models directly from LOLCODE.

```
CAN HAS BRAINZ?
```

### API

| Function | Arguments | Description |
|----------|-----------|-------------|
| `BRAINZ'Z KREEAYT` | config | Create a model from a config string |
| `BRAINZ'Z STUDEE` | model, text, lr | Run one training step, returns loss |
| `BRAINZ'Z SPEEK` | model, prompt, len, temp | Generate text from a prompt |
| `BRAINZ'Z SAVE` | model, filename | Save model weights to disk |
| `BRAINZ'Z LOAD` | filename | Load a model from disk |
| `BRAINZ'Z KTHXBAI` | model | Free model memory |

### Model Architecture

- **Transformer** with token + positional embeddings, causal self-attention,
  pre-norm layer normalization, and a two-layer FFN per block
- **Defaults**: 4 layers, 4 heads, 128-dim hidden, 512-dim FFN, 128-token
  vocab (ASCII), 64-token context window
- **Optimizer**: Adam with gradient clipping (max norm 1.0)
- **Tokenizer**: Character-level (byte ↔ token ID)

Config string keys: `layers`, `heads`, `hidden`, `ff`, `vocab`, `ctx`.

### Example — train a tiny model

```lolcode
HAI 1.4
  CAN HAS BRAINZ?
  CAN HAS STDLIB?

  BTW seed RNG
  STDLIB'Z MIX 1337

  BTW create a small model
  I HAS A mdl ITZ BRAINZ'Z KREEAYT "layers=2,heads=2,hidden=64,ff=256,ctx=32"

  BTW training loop
  I HAS A step ITZ 0
  IM IN YR loop UPPIN YR step TIL BOTH SAEM step AN 500
    I HAS A loss ITZ BRAINZ'Z STUDEE mdl AN "HAI WORLD KTHXBYE" AN 0.001
    BOTH SAEM MOD OF step AN 100 AN 0, O RLY?
      YA RLY, VISIBLE "step " AN step AN " loss " AN loss
    OIC
  IM OUTTA YR loop

  BTW generate
  I HAS A out ITZ BRAINZ'Z SPEEK mdl AN "HAI" AN 20 AN 0.8
  VISIBLE out

  BTW save and cleanup
  BRAINZ'Z SAVE mdl AN "model.brainz"
  BRAINZ'Z KTHXBAI mdl
KTHXBYE
```

BRAINZ also exposes low-level tensor operations (`TENSUR`, `TIEMZ`, `MOAR`,
`RELU`, `SOFT`, `BAKWARDZ`, `GETNUMBR`, `SHAEP`) for custom model
architectures.

For a full-scale LOLCODE language model project built on BRAINZ, see
[lollm](https://github.com/justinmeza/lollm).

## License

Copyright (C) 2010-2015 Justin J. Meza

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
