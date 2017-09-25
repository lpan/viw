# Viw, the ghetto editor

VI Worsened, a lightweight and fun VI clone. Inspired by the
[6-domino-cascade](https://github.com/Day8/re-frame#it-is-a-6-domino-cascade)
from the React world.

### DEMO
https://vimeo.com/205701269
## Dependencies

* gcc
* ncurses

```console
# Fedora
sudo dnf install ncurses-devel

# Debian/Ubuntu
sudo apt-get install libncurses5-dev
```

## Installation & usage

```console
git clone https://github.com/lpan/viw
cd viw/
make build
./viw [filename]
```

Using mingw compiler on Windows, you need to install `mingw-w64-x86_64-ncurses`

```console
pacman -S mingw-w64-x86_64-ncurses
mingw32-make build
```

### Supported keybindings

- `j` Cursor down
- `k` Cursor up
- `h` Cursor left
- `l` Cursor right
- `0` Cursor to the beginning of the line
- `$` Cursor to the end of the line
- `i` Insert before
- `I` Insert at the beginning of the line
- `a` Insert after
- `A` Insert at the end of the line
- `o` Append line then insert
- `O` Prepend line then insert
- `dd` Delete line under the cursor
- `gg` Go to the first line of the file
- `G` Go the last line of the file
- `u` Undo
- `r` Redo (**Unstable**)

### Supported EX mode commands
- `:q` quit
- `:w` save
- `:wq` save then quit

## Hacking

Feel free to contribute! :)

### How does it work

1. initiate the state
  - Read file to buffer.
  - Set up interface with ncurses
2. Listen to keyboard events.
  - Each supported keybinding is mapped to a method that mutates the `buffer`
3. Run `update_state(st)` and `render_update(st)`
  - Similar to `selectors` in redux, `update_state(state_t *st)` will update all the
    computed properties (such as cursor position, rows to be displayed on the screen, etc)
    according to the new mutated `buffer` state.
  - `render_update(state_t *st)` will actually render everything on the screen according to
    the result from `update_state()`.
4. Goto step 2

### Undo & Redo

Viw's undo & redo functionality is inspired by the `command pattern` and `event sourcing`.
It is hacky but it works and it is not that slow.
1. Initialization:
    * Deep clone the initial buffer.
    * Initialize two stacks (`history stack` and `redo stack`).
2. Capture all state-mutating functions and their payloads and push it on to the
   `history stack`.
3. When the user hits `undo`:
    * Pop the `history stack` and the push the result onto `redo stack`.
    * Clone the initial buffer and apply all the commands saved in the `history stack`
     on top of it.
4. When the user hits `redo`:
    * Pop the `redo stack` and apply the command immediately onto the current buffer.
5. Clear the `redo stack` when a command gets pushed to the `history stack` by the user.

See https://github.com/lpan/viw/blob/master/src/controller.c#L152 for more details

### Hierarchy of the states

Our main `state` object has two children states, namely `buffer` and `screen`. This seperation
makes it easier to perform unit tests against the `buffer`. It also facilitates the migration
to a different rendering library in the future.

#### The State

* The parent state stores computed states that depend on the `buffer` and/or `screen`. Those states
  include cursor positions, aount of space reserved for line numbers, etc. 

#### The Buffer

* The buffer is represented as a two dimensional doubly linked list. This
  allows conatant time line/char deletion and insertion. Go to [buffer.h](/src/buffer.h)
  for more information.
* Buffer is only allowed to be modified by the methods declared in `buffer.h`
  
#### The Screen

* The screen is the state of the interface between viw and GNU ncurses. It stores information
  such as pointers to the ncurses windows, etc. You can learn more about it at
  [screen.h](/src/screen.h).
