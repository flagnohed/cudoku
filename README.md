# cudoku

Usage: `make && ./cudoku \[-h\]`

`sudokus/{easy, medium, hard}.txt` are stolen from 
https://github.com/grantm/sudoku-exchange-puzzle-bank/tree/master

### Todo:
* Make constant numbers in another color do make it easier to 
differentiate them from user inputed numbers.
* Show difficulty rating (i.e. the float number that
is listed in the sudoku file)
* If solver succeeds, don't enter do-while loop waiting for 
user input before exiting (i.e. call `is_complete()` directly
after `solve()`)
* Run through a lot of sudokus to see which difficulty rating
the solver can handle at the moment (benchmark)

