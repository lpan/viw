# Viw, the ghetto editor

VI Worsened, a ghetto but fun VI clone.

## Dependencies
* gcc
* ncurses

## Installation & usage
```bash
$ cd viw/
$ make build
$ ./viw [filename]
```

## Hacking

Feel free to contribute! :)

### How does it work

1. Read file to buffer using `getline()`.
2. Listen to keyboard event.
3. Modify buffer -> update the screen.

#### buffer

* The buffer is a doubly linked list of `row`s and each `row` is a doubly linked
  list of `echar`s.
* The head of a `row` is always an `NULL` `echar` node.

*Note:* Doubly linked list makes this implementation easy and efficient. Since the
user only wants to insert line/char next to the cursor, we can use a `current`
node to determine where to insert line/char. As the user moves the cursor, we
reassign the value of the `current` node.
